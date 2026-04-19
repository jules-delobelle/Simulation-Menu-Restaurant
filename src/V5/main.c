#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "fonctions_communication.h"

void *routine_client(void *arg);
void *routine_dataserveur(void *arg);
void *routine_routeur(void *arg);

int main()
{
#ifdef DATASERVEUR
    pthread_t thread_dataserveur;
    pthread_create(&thread_dataserveur, NULL, routine_dataserveur, NULL);
    pthread_join(thread_dataserveur, NULL);
#endif

#ifdef ROUTEUR
    pthread_t thread_routeur;
    pthread_create(&thread_routeur, NULL, routine_routeur, NULL);
    pthread_join(thread_routeur, NULL);
#endif

#ifdef CLIENT
    FILE *f = fopen("menus.txt", "r");
    char lignes[10][256];
    int nb_lignes = 0;

    /* Lit toutes les lignes et construit les commandes */
    while (fgets(lignes[nb_lignes], 256, f) && nb_lignes < 10)
    {
        int code_lieu, code_menu;
        sscanf(lignes[nb_lignes], "%d %d", &code_lieu, &code_menu);
        /* Reformate en |0|code_lieu|code_menu| */
        snprintf(lignes[nb_lignes], 256, "|0|%d|%d|", code_lieu, code_menu);
        nb_lignes++;
    }
    fclose(f);

    pthread_t threads[10];

    /* Lance tous les threads */
    for (int i = 0; i < nb_lignes; i++)
    {
        usleep(200000); /* 200ms entre chaque pour laisser le routeur publier */
        pthread_create(&threads[i], NULL, routine_client, lignes[i]);
    }

    /* Attend qu'ils finissent tous */
    for (int i = 0; i < nb_lignes; i++)
        pthread_join(threads[i], NULL);
#endif

    return 0;
}