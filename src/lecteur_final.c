#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

#define BUFFER_LENGTH 100

int charger_menu(RequeteQR *requete, char *resultat){
    FILE *file;
    file = fopen("menus.txt", "r");
    if(file == NULL){
        fprintf(stderr, "Erreur : fichier introuvable\n");
        return 0;
    }
    char line[100];
    while(fgets(line, 100, file)){
        int code_lieu, code_menu;
        char nom[100];
        sscanf(line, "%d %d \"%[^\"]\"", &code_lieu, &code_menu, nom);
        if(requete->code_lieu == code_lieu && requete->code_menu == code_menu){
            strcpy(resultat, nom);
            return 1;
        }
    }
    fprintf(stderr, "Erreur : reference menu inexistante");
    return 0;
}

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

    RequeteQR requete;
    char buffer[BUFFER_LENGTH];
    if(parser_requete(message_relais_recu, &requete) != 3){
        fprintf(stderr, "Erreur : contenu mal parsé");
        return 0;
    }
    if(!charger_menu(&requete, buffer)){
        fprintf(stderr, "Erreur : erreur lors du chargement du menu");
        return 0;
    }

    // ouverture du pipe de confirmation et envoie de l'accusé de reception
    int fd_recu_confirmation_relais = open(com_vers_lecteur_final.pipe_response, O_WRONLY);
    if (fd_recu_confirmation_relais == -1)
    {
        perror("Erreur: Le pipe de confirmation n'a pas été créé !\n");
        return 1;
    }
    write(fd_recu_confirmation_relais, buffer, strlen(buffer) + 1);
    close(fd_recu_confirmation_relais);
    return 0;
}