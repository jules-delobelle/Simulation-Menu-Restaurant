#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

void *routine_client(void *arg)
{
    LienCommunication com_vers_routeur;
    char message[256];
    char confirmation[256];

    /* Attend que le routeur publie son ID puis se connecte */
    connection(&com_vers_routeur, "client", "routeur");

    if (arg != NULL)
    {
        strncpy(message, (char *)arg, 255);
        printf("Envoi de la commande : %s\n", message);
        fflush(stdout);
        ecriture_pipe(&com_vers_routeur, message);
    }
    else
    {
        /* saisie clavier */
        ecriture_textuelle_pipe(&com_vers_routeur, message);
    }

    /* Attend la réponse */
    lecture_confirmation(&com_vers_routeur, confirmation);

    return NULL;
}