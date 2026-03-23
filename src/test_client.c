#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
    // initialisation lien pipe et buffer des messages
    LienCommunication com_vers_lecteur;
    char message_client[256];
    char confirmation[256];

    // jointure pipe client -> lecteur
    printf("En attente de connexion avec le lecteur...\n\n");
    rejoindre_pipe(&com_vers_lecteur, "client", "/tmp/id_for_client_lecteur_com.fifo");
    printf("Connection effectuée avec le lecteur sur le pipe %d\n\n", com_vers_lecteur.id_pipe);

    // ouverture pipe client -> lecteur
    int fd_client = open(com_vers_lecteur.pipe_request, O_WRONLY);
    if (fd_client == -1)
    {
        perror("Erreur: Le pipe de communication n'existe pas encore!\n\n");
        return 1;
    }
    printf("Entrez un message à envoyer au lecteur : \n");
    fgets(message_client, sizeof(message_client), stdin);
    write(fd_client, message_client, sizeof(message_client));
    close(fd_client);
    printf("Écrivain : Message envoyé.\n\n");

    // ouverture du pipe de confirmation de reception du lecteur
    int fd_return = open(com_vers_lecteur.pipe_response, O_RDONLY);
    if (fd_return != -1)
    {
        read(fd_return, confirmation, sizeof(confirmation));
        printf("Confirmation: %s.\n\n", confirmation);
        close(fd_return);
    }

    return 0;
}