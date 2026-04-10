#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_communication.h"

int main()
{
   char message[256];
   char confirmation[256];
   LienCommunication com_vers_client;
   LienCommunication com_vers_lecteur_final;

   /* Crée les FIFOs côté client et publie l'ID */
   initialiser_pipe(&com_vers_client, "relais");
   publier_id_pipe(&com_vers_client, "relais", "client");

   /* Se connecte au lecteur_final (qui a déjà publié son ID) */
   connection(&com_vers_lecteur_final, "relais", "lecteur_final");

   /* Reçoit le message du client */
   lecture_pipe(&com_vers_client, message);

   /* Transmet au lecteur_final */
   ecriture_pipe(&com_vers_lecteur_final, message);

   /* Attend la confirmation du lecteur_final */
   lecture_confirmation(&com_vers_lecteur_final, confirmation);

   return 0;
}