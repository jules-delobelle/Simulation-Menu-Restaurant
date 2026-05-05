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
    pthread_t thread_ds[3];
    char *fichiers[3] = {"menus_lieu1.txt", "menus_lieu2.txt", "menus_lieu3.txt"};

    pthread_create(&thread_ds[0], NULL, routine_dataserveur, fichiers[0]);
    pthread_create(&thread_ds[1], NULL, routine_dataserveur, fichiers[1]);
    pthread_create(&thread_ds[2], NULL, routine_dataserveur, fichiers[2]);

    pthread_join(thread_ds[0], NULL);
    pthread_join(thread_ds[1], NULL);
    pthread_join(thread_ds[2], NULL);
#endif

#ifdef ROUTEUR
    pthread_t thread_routeur;
    pthread_create(&thread_routeur, NULL, routine_routeur, NULL);
    pthread_join(thread_routeur, NULL);
#endif

#ifdef CLIENT
    FILE *f;
    char lignes[30][256];
    int nb_lignes = 0;
    char *fichiers_menus[3] = {"menus_lieu1.txt", "menus_lieu2.txt", "menus_lieu3.txt"};

    /* Lit toutes les lignes de tous les fichiers menus */
    for (int j = 0; j < 3; j++)
    {
        f = fopen(fichiers_menus[j], "r");
        if (f == NULL)
        {
            fprintf(stderr, "Erreur : %s introuvable\n", fichiers_menus[j]);
            continue;
        }
        while (fgets(lignes[nb_lignes], 256, f) && nb_lignes < 30)
        {
            int code_lieu, code_menu;
            sscanf(lignes[nb_lignes], "%d %d", &code_lieu, &code_menu);
            snprintf(lignes[nb_lignes], 256, "|0|%d|%d|", code_lieu, code_menu);
            nb_lignes++;
        }
        fclose(f);
    }

    pthread_t threads[30];

    /* Lance tous les threads clients */
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