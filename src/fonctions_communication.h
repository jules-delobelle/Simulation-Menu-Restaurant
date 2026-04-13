#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef FONCTIONS_COMMUNICATION_H
#define FONCTIONS_COMMUNICATION_H

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

void initialiser_pipe(LienCommunication *Lien, const char *communication_source);

void publier_id_pipe(LienCommunication *Lien, const char *handshake_path);

void rejoindre_pipe(LienCommunication *Lien, const char *source, const char *handshake_path);

int parser_requete(const char *requete_str, RequeteQR *requete);

int valider_format_requete(const char *requete_str);

#endif
