#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "fonctions_communication.h"

/*
 * Crée deux FIFOs nommés d'après nom_canal + ID aléatoire.
 * Ouvre pipe_request en O_RDWR et le garde ouvert (fd stocké dans un
 * fichier temporaire via fork non — on le garde dans une variable statique
 * locale non, on va faire plus simple) :
 *
 * APPROCHE FINALE :
 * - Le handshake passe par un fichier normal (pas un FIFO) → jamais bloquant
 * - Le FIFO de requête est ouvert en O_RDWR dès sa création et ce fd
 *   est conservé dans fd_rdwr (champ caché, on le stocke via une astuce :
 *   on utilise un tableau global indexé par id_pipe)
 *
 * Plus simple encore : on stocke le fd dans un fichier /tmp/fd_NOM_ID
 * Non — encore plus simple : on garde juste un fd global par canal.
 *
 * SOLUTION VRAIMENT SIMPLE :
 * initialiser_pipe ouvre le FIFO en O_RDWR et stocke le fd dans pipe_response[0]
 * Non...
 *
 * OK : on ajoute juste un champ fd_rdwr dans la struct, caché de l'utilisateur
 * via le .h propre. On l'initialise dans initialiser_pipe et on le ferme dans lecture_pipe.
 */

/* fd_rdwr gardé ouvert pour débloquer les open(O_WRONLY) de l'écrivain */
static int g_fd_rdwr[2] = {-1, -1}; /* max 2 canaux ouverts en même temps */
static char g_canal[2][256] = {"", ""};

static void stocker_fd(const char *canal, int fd)
{
    for (int i = 0; i < 2; i++)
    {
        if (g_fd_rdwr[i] == -1 || strcmp(g_canal[i], canal) == 0)
        {
            g_fd_rdwr[i] = fd;
            strncpy(g_canal[i], canal, 255);
            return;
        }
    }
}

static int recuperer_fd(const char *canal)
{
    for (int i = 0; i < 2; i++)
    {
        if (strcmp(g_canal[i], canal) == 0)
            return g_fd_rdwr[i];
    }
    return -1;
}

static void liberer_fd(const char *canal)
{
    for (int i = 0; i < 2; i++)
    {
        if (strcmp(g_canal[i], canal) == 0)
        {
            g_fd_rdwr[i] = -1;
            g_canal[i][0] = '\0';
            return;
        }
    }
}

/* ------------------------------------------------------------------ */

/*
 * Crée les deux FIFOs (requête + réponse) avec un ID aléatoire.
 * nom_canal : nom court qui identifie ce canal (ex: "relais", "client")
 * Les pipes seront nommés /tmp/NOM_ID_req.fifo et /tmp/NOM_ID_res.fifo
 *
 * Ouvre pipe_request en O_RDWR immédiatement : tant que ce fd est ouvert,
 * un open(O_WRONLY) de l'autre côté ne bloque pas.
 */
void initialiser_pipe(LienCommunication *Lien, const char *nom_canal)
{
    srand(time(NULL) ^ getpid());
    Lien->id_pipe = (rand() % 9000) + 1000;

    snprintf(Lien->pipe_request, 256, "/tmp/%s_%d_req.fifo", nom_canal, Lien->id_pipe);
    snprintf(Lien->pipe_response, 256, "/tmp/%s_%d_res.fifo", nom_canal, Lien->id_pipe);

    mkfifo(Lien->pipe_request, 0666);
    mkfifo(Lien->pipe_response, 0666);

    /* Garde le FIFO ouvert en lecture/écriture pour ne pas bloquer l'écrivain */
    int fd = open(Lien->pipe_request, O_RDWR);
    stocker_fd(Lien->pipe_request, fd);
}

/*
 * Publie l'ID du pipe dans un fichier normal (pas un FIFO).
 * Un fichier normal ne bloque jamais à l'ouverture.
 * Convention : /tmp/from_SOURCE_to_TARGET
 */
void publier_id_pipe(LienCommunication *Lien, const char *source, const char *target)
{
    char handshake_path[256];
    snprintf(handshake_path, 256, "/tmp/from_%s_to_%s", source, target);
    int fd = open(handshake_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd, &Lien->id_pipe, sizeof(int));
    close(fd);
}

/*
 * Attend le fichier de handshake publié par le serveur,
 * lit l'ID et reconstruit les chemins des FIFOs.
 * nom_canal côté serveur = target (ex: "relais" si on se connecte au relais)
 */
void rejoindre_pipe(LienCommunication *Lien, const char *source, const char *target)
{
    char handshake_path[256];
    snprintf(handshake_path, 256, "/tmp/from_%s_to_%s", target, source);

    /* Attend que le serveur publie le fichier */
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

/* Attend que le serveur publie son ID puis se connecte */
void connection(LienCommunication *Lien, const char *source, const char *target)
{
    printf("En attente de connexion avec le %s...\n\n", target);
    fflush(stdout);
    rejoindre_pipe(Lien, source, target);
    printf("Connexion effectuée avec le %s sur le pipe %d\n\n", target, Lien->id_pipe);
    fflush(stdout);
}

/*
 * Lit un message depuis pipe_request.
 * Utilise le fd O_RDWR gardé ouvert (pour ne pas bloquer l'écrivain pendant read).
 * Envoie automatiquement une confirmation.
 */
void lecture_pipe(LienCommunication *Lien, char *message_recu)
{
    /* On ouvre un fd O_RDONLY séparé pour le read — l'écrivain peut ouvrir
       en O_WRONLY car le fd O_RDWR est toujours ouvert */
    int fd = open(Lien->pipe_request, O_RDONLY);
    read(fd, message_recu, 256);
    close(fd);

    /* Maintenant on ferme le fd O_RDWR : plus besoin */
    int fd_rdwr = recuperer_fd(Lien->pipe_request);
    if (fd_rdwr != -1)
    {
        close(fd_rdwr);
        liberer_fd(Lien->pipe_request);
    }

    printf("Le lecteur a recu: %s\n", message_recu);
    fflush(stdout);

    /* Envoi de la confirmation */
    int fd_res = open(Lien->pipe_response, O_WRONLY);
    if (fd_res == -1)
    {
        perror("lecture_pipe: confirmation");
        return;
    }
    const char *conf = "Message bien recu";
    write(fd_res, conf, strlen(conf) + 1);
    close(fd_res);
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
    /* fgets AVANT open : on lit stdin d'abord, puis on ouvre le FIFO.
       Le fd O_RDWR du serveur est déjà ouvert donc open(O_WRONLY) ne bloque pas. */
    printf("Entrez un message à envoyer au lecteur : \n");
    fflush(stdout);
    fgets(message, 256, stdin);

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