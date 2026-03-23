#include <stdio.h>
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

void initialiser_pipe(LienCommunication *Lien, const char *communication_source);

void publier_id_pipe(LienCommunication *Lien, const char *handshake_path);

void rejoindre_pipe(LienCommunication *Lien, const char *source, const char *handshake_path);

#endif
