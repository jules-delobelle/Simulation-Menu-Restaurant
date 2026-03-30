#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
    // initialise le pipe avec le client
    char message_recu[256];
    char message_relais[256];
    char confirmation_relais[256];
    LienCommunication com_vers_lecteur;
    LienCommunication com_vers_lecteur_final;

    // jointure pipe lecteur -> lecteur final
    printf("En attente de connexion avec le lecteur final...\n\n");
    rejoindre_pipe(&com_vers_lecteur_final, "relais", "/tmp/id_for_relais_lecteur_final_com.fifo");
    printf("Connection effectuée avec le lecteur final sur le pipe %d\n\n", com_vers_lecteur_final.id_pipe);

    // initialise le pipe de communication avec le client
    initialiser_pipe(&com_vers_lecteur, "client");
    publier_id_pipe(&com_vers_lecteur, "/tmp/id_for_client_lecteur_com.fifo");

    // ouvre le pipe pour lire la commande tapée par le client
    int fd_recu = open(com_vers_lecteur.pipe_request, O_RDONLY);
    read(fd_recu, message_recu, sizeof(message_recu));
    printf("Le lecteur a recu: %s\n", message_recu);
    close(fd_recu);

    // ouverture du pipe de confirmation et envoie de l'accusé de reception
    int fd_recu_confirmation = open(com_vers_lecteur.pipe_response, O_WRONLY);
    if (fd_recu_confirmation == -1)
    {
        perror("Erreur: Le pipe de confirmation n'a pas été créé !\n\n");
        return 1;
    }
    char *message_confirmation = "Message bien recu par le lecteur";
    write(fd_recu_confirmation, message_confirmation, strlen(message_confirmation) + 1);
    close(fd_recu_confirmation);

    // ouverture pipe lecteur -> lecteur final
    int fd_relais = open(com_vers_lecteur_final.pipe_request, O_WRONLY);
    if (fd_relais == -1)
    {
        perror("Erreur: Le pipe de communication n'existe pas encore!\n\n");
        return 1;
    }
    write(fd_relais, message_recu, sizeof(message_recu));
    close(fd_relais);

    // ouverture du pipe de confirmation de reception du lecteur final
    int fd_return = open(com_vers_lecteur_final.pipe_response, O_RDONLY);
    if (fd_return != -1)
    {
        read(fd_return, confirmation_relais, sizeof(confirmation_relais));
        printf("Confirmation: %s.\n\n", confirmation_relais);
        close(fd_return);
    }

    return 0;
}