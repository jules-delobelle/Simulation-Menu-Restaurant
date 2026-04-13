#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "fonctions_communication.h"

/* Gestion interne du fd O_RDWR (évite le blocage
   quand l'écrivain ouvre en O_WRONLY) */
static int fd_rdwr[2] = {-1, -1};
static char noms_canaux[2][256] = {"", ""};

static void stocker_fd(const char *canal, int fd)
{
    for (int i = 0; i < 2; i++)
    {
        if (fd_rdwr[i] == -1 || strcmp(noms_canaux[i], canal) == 0)
        {
            fd_rdwr[i] = fd;
            strncpy(noms_canaux[i], canal, 255);
            return;
        }
    }
}

static int recuperer_fd(const char *canal)
{
    for (int i = 0; i < 2; i++)
        if (strcmp(noms_canaux[i], canal) == 0)
            return fd_rdwr[i];
    return -1;
}

static void liberer_fd(const char *canal)
{
    for (int i = 0; i < 2; i++)
    {
        if (strcmp(noms_canaux[i], canal) == 0)
        {
            fd_rdwr[i] = -1;
            noms_canaux[i][0] = '\0';
            return;
        }
    }
}

/* Initialisation et handshake */

void initialiser_pipe(LienCommunication *Lien, const char *nom_canal)
{
    srand(time(NULL) ^ getpid());
    Lien->id_pipe = (rand() % 9000) + 1000;

    snprintf(Lien->pipe_request, 256, "/tmp/%s_%d_req.fifo", nom_canal, Lien->id_pipe);
    snprintf(Lien->pipe_response, 256, "/tmp/%s_%d_res.fifo", nom_canal, Lien->id_pipe);

    mkfifo(Lien->pipe_request, 0666);
    mkfifo(Lien->pipe_response, 0666);

    /* Garde le FIFO ouvert en O_RDWR pour ne pas bloquer l'écrivain */
    int fd = open(Lien->pipe_request, O_RDWR);
    stocker_fd(Lien->pipe_request, fd);
}

/* Publie l'ID dans un fichier normal (pas un FIFO) -> jamais bloquant.
 * Convention du nom : /tmp/from_SOURCE_to_TARGET */
void publier_id_pipe(LienCommunication *Lien, const char *source, const char *target)
{
    char handshake_path[256];
    snprintf(handshake_path, 256, "/tmp/from_%s_to_%s", source, target);
    int fd = open(handshake_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd, &Lien->id_pipe, sizeof(int));
    close(fd);
}

/* Attend le fichier de handshake publié par le serveur,
 * lit l'ID et reconstruit les chemins des FIFOs.*/
void rejoindre_pipe(LienCommunication *Lien, const char *source, const char *target)
{
    char handshake_path[256];
    snprintf(handshake_path, 256, "/tmp/from_%s_to_%s", target, source);

    /* Attente active que le serveur publie le fichier */
    while (access(handshake_path, F_OK) == -1)
        usleep(5000);

    int fd = open(handshake_path, O_RDONLY);
    read(fd, &Lien->id_pipe, sizeof(int));
    close(fd);

    /* Les FIFOs ont été créés par le serveur sous le nom "target" */
    snprintf(Lien->pipe_request, 256, "/tmp/%s_%d_req.fifo", target, Lien->id_pipe);
    snprintf(Lien->pipe_response, 256, "/tmp/%s_%d_res.fifo", target, Lien->id_pipe);

    /* Attend que les FIFOs existent */
    while (access(Lien->pipe_request, F_OK) == -1)
        usleep(5000);
    while (access(Lien->pipe_response, F_OK) == -1)
        usleep(5000);
}

/* Wrapper avec message de log */
void connection(LienCommunication *Lien, const char *source, const char *target)
{
    printf("En attente de connexion avec le %s...\n\n", target);
    fflush(stdout);
    rejoindre_pipe(Lien, source, target);
    printf("Connexion effectuée avec le %s sur le pipe %d\n\n", target, Lien->id_pipe);
    fflush(stdout);
}

/* Envoi et réception */

/* Lit un message depuis pipe_request.
 * Envoie automatiquement "Message bien recu" dans pipe_response. */
void lecture_pipe(LienCommunication *Lien, char *message_recu)
{
    int fd = open(Lien->pipe_request, O_RDONLY);
    read(fd, message_recu, 256);
    close(fd);

    /* Retire le \n s'il y en a un */
    message_recu[strcspn(message_recu, "\n")] = '\0';

    /* Ferme le fd O_RDWR maintenant qu'on a lu */
    int fd_rdwr = recuperer_fd(Lien->pipe_request);
    if (fd_rdwr != -1)
    {
        close(fd_rdwr);
        liberer_fd(Lien->pipe_request);
    }

    printf("Le lecteur a recu: %s\n", message_recu);
    fflush(stdout);

    /* Pas de confirmation automatique : c'est le relais qui gère le retour */
}

/* Attend et affiche la confirmation */
void lecture_confirmation(LienCommunication *Lien, char *confirmation)
{
    int fd = open(Lien->pipe_response, O_RDONLY);
    read(fd, confirmation, 256);
    printf("Confirmation: %s\n\n", confirmation);
    fflush(stdout);
    close(fd);
}

/* Envoie un message déjà en mémoire */
void ecriture_pipe(LienCommunication *Lien, char *message)
{
    int fd = open(Lien->pipe_request, O_WRONLY);
    if (fd == -1)
    {
        perror("ecriture_pipe");
        return;
    }
    write(fd, message, strlen(message) + 1);
    close(fd);
}

/* Saisit un message au clavier puis l'envoie */
void ecriture_textuelle_pipe(LienCommunication *Lien, char *message)
{
    printf("Entrez une commande au format |code_serveur|code_lieu|code_menu| :\n");
    fflush(stdout);
    fgets(message, 256, stdin);
    message[strcspn(message, "\n")] = '\0'; /* retire le \n */

    int fd = open(Lien->pipe_request, O_WRONLY);
    if (fd == -1)
    {
        perror("ecriture_textuelle_pipe");
        return;
    }
    write(fd, message, strlen(message) + 1);
    close(fd);
    fflush(stdout);
}

/* Parsing et validation de la requête QR */

int parser_requete(const char *requete_str, RequeteQR *requete)
{
    return sscanf(requete_str, "|%d|%d|%d|", &requete->code_serveur, &requete->code_lieu, &requete->code_menu);
}

int valider_format_requete(const char *requete_str)
{
    RequeteQR requete;
    if (parser_requete(requete_str, &requete) != 3)
    {
        fprintf(stderr, "Erreur : format invalide\n");
        return 0;
    }
    return 1;
}