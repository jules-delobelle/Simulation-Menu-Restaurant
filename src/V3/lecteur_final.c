#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
    // initialise le pipe avec le relais
    char message_relais_recu[256];
    LienCommunication com_vers_lecteur_final;
    initialiser_pipe(&com_vers_lecteur_final, "relais");
    publier_id_pipe(&com_vers_lecteur_final, "/tmp/id_for_relais_lecteur_final_com.fifo");

    // ouvre le pipe pour lire la commande relayée par le relais
    int fd_recu = open(com_vers_lecteur_final.pipe_request, O_RDONLY);
    read(fd_recu, message_relais_recu, sizeof(message_relais_recu));
    printf("Le lecteur a relayé le message: %s\n", message_relais_recu);
    close(fd_recu);

    // ouverture du pipe de confirmation et envoie de l'accusé de reception
    int fd_recu_confirmation_relais = open(com_vers_lecteur_final.pipe_response, O_WRONLY);
    if (fd_recu_confirmation_relais == -1)
    {
        perror("Erreur: Le pipe de confirmation n'a pas été créé !\n");
        return 1;
    }
    char *message_confirmation_relais = "Message bien relayé au lecteur final";
    write(fd_recu_confirmation_relais, message_confirmation_relais, strlen(message_confirmation_relais) + 1);
    close(fd_recu_confirmation_relais);
    return 0;
}