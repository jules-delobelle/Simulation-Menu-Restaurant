#ifndef FONCTIONS_COMMUNICATION_H
#define FONCTIONS_COMMUNICATION_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    int id_pipe;
    char pipe_request[256];
    char pipe_response[256];
} LienCommunication;

typedef struct {
    int code_serveur;
    int code_lieu;
    int code_menu;
} RequeteQR;

/* Côté serveur : crée les FIFOs et publie l'ID */
void initialiser_pipe(LienCommunication *Lien, const char *nom_canal);
void publier_id_pipe(LienCommunication *Lien, const char *source, const char *target);

/* Côté client : récupère l'ID et se connecte */
void rejoindre_pipe(LienCommunication *Lien, const char *source, const char *target);
void connection(LienCommunication *Lien, const char *source, const char *target);

/* Envoi / réception */
void lecture_pipe(LienCommunication *Lien, char *message_recu);
void lecture_confirmation(LienCommunication *Lien, char *confirmation);
void ecriture_pipe(LienCommunication *Lien, char *message);
void ecriture_textuelle_pipe(LienCommunication *Lien, char *message);

/* Parsing et validation de la requête QR */
int parser_requete(const char *requete_str, RequeteQR *requete);
int valider_format_requete(const char *requete_str);

#endif
