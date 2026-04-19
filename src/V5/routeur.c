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

    /* connexion au dataserveur une seule fois */
    LienCommunication com_vers_dataserveur;
    connection(&com_vers_dataserveur, "routeur", "dataserveur");

    while (1)
    {
        char message_recu[256];
        char confirmation[256];
        LienCommunication com_vers_client;

        char handshake[256];
        snprintf(handshake, 256, "/tmp/from_routeur_to_client");
        unlink(handshake);

        initialiser_pipe(&com_vers_client, "routeur");
        publier_id_pipe(&com_vers_client, "routeur", "client");

        sem_wait(sem);

        lecture_pipe(&com_vers_client, message_recu);

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

        ecriture_pipe(&com_vers_dataserveur, message_recu);
        lecture_confirmation(&com_vers_dataserveur, confirmation);

        int fd = open(com_vers_client.pipe_response, O_WRONLY);
        if (fd == -1)
        {
            perror("routeur: renvoi client");
            sem_post(sem);
            continue;
        }
        write(fd, confirmation, strlen(confirmation) + 1);
        close(fd);

        sem_post(sem);
    }

    return NULL;
}