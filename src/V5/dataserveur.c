#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

void *routine_dataserveur(void *arg)
{
    (void)arg;
    LienCommunication com_vers_routeur;

    /* Crée les FIFOs et publie l'ID au routeur une seule fois */
    initialiser_pipe(&com_vers_routeur, "dataserveur");
    publier_id_pipe(&com_vers_routeur, "dataserveur", "routeur");

    while (1)
    {
        char message_recu[BUFFER_LENGTH];
        char menu_trouve[BUFFER_LENGTH];

        /* Attend et lit la commande du routeur */
        lecture_pipe(&com_vers_routeur, message_recu);

        /* Parse la requête */
        RequeteQR requete;
        if (parser_requete(message_recu, &requete) != 3)
        {
            fprintf(stderr, "Erreur : contenu mal parsé\n");
            continue;
        }

        /* Charge le menu */
        if (!charger_menu(&requete, menu_trouve))
            snprintf(menu_trouve, BUFFER_LENGTH, "Erreur : menu introuvable");

        printf("Menu trouvé : %s\n", menu_trouve);
        fflush(stdout);

        /* Renvoie le résultat au routeur */
        int fd = open(com_vers_routeur.pipe_response, O_WRONLY);
        if (fd == -1)
        {
            perror("dataserveur: réponse routeur");
            continue;
        }
        write(fd, menu_trouve, strlen(menu_trouve) + 1);
        close(fd);
    }

    return NULL;
}