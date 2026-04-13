#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
    char message_recu[256];
    char confirmation[256];
    LienCommunication com_vers_client;
    LienCommunication com_vers_lecteur_final;

    /* 1. Se connecte au lecteur_final (qui a déjà publié son ID) */
    connection(&com_vers_lecteur_final, "relais", "lecteur_final");

    /* 2. Crée les FIFOs côté client et publie l'ID */
    initialiser_pipe(&com_vers_client, "relais");
    publier_id_pipe(&com_vers_client, "relais", "client");

    /* 3. Reçoit la commande du client */
    lecture_pipe(&com_vers_client, message_recu);
    /* lecture_pipe envoie automatiquement "Message bien recu" au client */

    /* 4. Valide le format de la requête */
    if (!valider_format_requete(message_recu))
    {
        /* Envoie l'erreur au client via pipe_response */
        int fd_err = open(com_vers_client.pipe_response, O_WRONLY);
        if (fd_err != -1)
        {
            char msg_erreur[] = "Erreur : format invalide, attendu |code_serveur|code_lieu|code_menu|";
            write(fd_err, msg_erreur, strlen(msg_erreur) + 1);
            close(fd_err);
        }
        return 1;
    }

    /* 5. Transmet la commande au lecteur_final */
    ecriture_pipe(&com_vers_lecteur_final, message_recu);

    /* 6. Attend la réponse du lecteur_final (nom du menu) */
    lecture_confirmation(&com_vers_lecteur_final, confirmation);

    /* 7. Renvoie la réponse au client */
    int fd = open(com_vers_client.pipe_response, O_WRONLY);
    if (fd == -1) { perror("relais: renvoi client"); return 1; }
    write(fd, confirmation, strlen(confirmation) + 1);
    close(fd);

    return 0;
}
