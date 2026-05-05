#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include "fonctions_communication.h"

static sem_t *sem;

void *routine_routeur(void *arg)
{
    (void)arg;
    sem = sem_open("/verrou_routeur", O_CREAT, 0666, 1);

    /* connexion aux 3 dataserveurs une seule fois avant la boucle */
    LienCommunication com_dataserveur[3];
    connection(&com_dataserveur[0], "routeur", "menus_lieu1.txt");
    connection(&com_dataserveur[1], "routeur", "menus_lieu2.txt");
    connection(&com_dataserveur[2], "routeur", "menus_lieu3.txt");

    while (1)
    {
        char message_recu[256];
        char confirmation[256];
        char menu_final[256];
        LienCommunication com_vers_client;

        /* Attend que le client précédent ait consommé le handshake */
        while (access("/tmp/from_routeur_to_client", F_OK) == 0)
            usleep(5000);

        initialiser_pipe(&com_vers_client, "routeur");
        publier_id_pipe(&com_vers_client, "routeur", "client");

        /* Prend le verrou */
        sem_wait(sem);

        /* Reçoit la commande du client */
        lecture_pipe(&com_vers_client, message_recu);

        /* Valide le format */
        if (!valider_format_requete(message_recu))
        {
            int fd_err = open(com_vers_client.pipe_response, O_WRONLY);
            if (fd_err != -1)
            {
                char msg_erreur[] = "Erreur : format invalide, attendu |code_serveur|code_lieu|code_menu|";
                write(fd_err, msg_erreur, strlen(msg_erreur) + 1);
                close(fd_err);
            }
            sem_post(sem);
            continue;
        }

        /* Interroge les dataserveurs un par un */
        snprintf(menu_final, 256, "Erreur : aucun menu correspondant");
        for (int i = 0; i < 3; i++)
        {
            ecriture_pipe(&com_dataserveur[i], message_recu);
            lecture_confirmation(&com_dataserveur[i], confirmation);
            if (strcmp(confirmation, "NON") != 0)
            {
                strncpy(menu_final, confirmation, 255);
                break;
            }
        }

        /* Renvoie le résultat au client */
        int fd = open(com_vers_client.pipe_response, O_WRONLY);
        if (fd == -1)
        {
            perror("routeur: renvoi client");
            sem_post(sem);
            continue;
        }
        write(fd, menu_final, strlen(menu_final) + 1);
        close(fd);

        /* Libère le verrou */
        sem_post(sem);
    }

    return NULL;
}