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

    /* Saisit et envoie la commande au format |code_serveur|code_lieu|code_menu| */
    ecriture_textuelle_pipe(&com_vers_relais, message);

    /* Attend la réponse (nom du menu ou message d'erreur) */
    lecture_confirmation(&com_vers_relais, confirmation);

    return 0;
}
