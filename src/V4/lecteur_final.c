#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

#define BUFFER_LENGTH 256

int charger_menu(RequeteQR *requete, char *resultat)
{
    FILE *file = fopen("menus.txt", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Erreur : fichier menus.txt introuvable\n");
        return 0;
    }

    char line[BUFFER_LENGTH];
    while (fgets(line, BUFFER_LENGTH, file))
    {
        int  code_lieu, code_menu;
        char nom[BUFFER_LENGTH];
        sscanf(line, "%d %d \"%[^\"]\"", &code_lieu, &code_menu, nom);
        if (requete->code_lieu == code_lieu && requete->code_menu == code_menu)
        {
            strcpy(resultat, nom);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    fprintf(stderr, "Erreur : référence menu inexistante\n");
    return 0;
}

int main()
{
    char message_recu[BUFFER_LENGTH];
    char menu_trouve[BUFFER_LENGTH];
    LienCommunication com_vers_relais;

    /* 1. Crée les FIFOs et publie l'ID au relais */
    initialiser_pipe(&com_vers_relais, "lecteur_final");
    publier_id_pipe(&com_vers_relais, "lecteur_final", "relais");

    /* 2. Attend et lit la commande transmise par le relais */
    lecture_pipe(&com_vers_relais, message_recu);
    /* lecture_pipe envoie automatiquement "Message bien recu" — on écrasera avec le vrai résultat */

    /* 3. Parse la requête */
    RequeteQR requete;
    if (parser_requete(message_recu, &requete) != 3)
    {
        fprintf(stderr, "Erreur : contenu mal parsé\n");
        return 1;
    }

    /* 4. Charge le menu correspondant */
    if (!charger_menu(&requete, menu_trouve))
    {
        snprintf(menu_trouve, BUFFER_LENGTH, "Erreur : menu introuvable");
    }

    printf("Menu trouvé : %s\n", menu_trouve);
    fflush(stdout);

    /* 5. Renvoie le nom du menu au relais via pipe_response */
    int fd = open(com_vers_relais.pipe_response, O_WRONLY);
    if (fd == -1) { perror("lecteur_final: réponse relais"); return 1; }
    write(fd, menu_trouve, strlen(menu_trouve) + 1);
    close(fd);

    return 0;
}
