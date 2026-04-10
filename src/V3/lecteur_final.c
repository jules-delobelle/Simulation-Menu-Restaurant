#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
    char message[256];
    LienCommunication com_depuis_relais;

    /* Crée les FIFOs et publie l'ID au relais */
    initialiser_pipe(&com_depuis_relais, "lecteur_final");
    publier_id_pipe(&com_depuis_relais, "lecteur_final", "relais");

    /* Attend le message du relais, envoie automatiquement la confirmation */
    lecture_pipe(&com_depuis_relais, message);

    return 0;
}