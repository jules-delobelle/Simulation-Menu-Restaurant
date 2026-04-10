#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
    LienCommunication com_vers_relais;
    char message[256];
    char confirmation[256];

    /* Attend que le relais publie son ID puis se connecte */
    connection(&com_vers_relais, "client", "relais");

    /* Saisit et envoie le message */
    ecriture_textuelle_pipe(&com_vers_relais, message);

    /* Attend la confirmation */
    lecture_confirmation(&com_vers_relais, confirmation);

    return 0;
}