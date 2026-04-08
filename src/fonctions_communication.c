#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fonctions_communication.h"

void initialiser_pipe(LienCommunication *Lien, const char *communication_source)
{
    srand(time(NULL) ^ getpid());
    Lien->id_pipe = (rand() % 9000) + 1000;

    snprintf(Lien->pipe_request, 256, "/tmp/%s_%d_req.fifo", communication_source, Lien->id_pipe);
    snprintf(Lien->pipe_response, 256, "/tmp/%s_%d_res.fifo", communication_source, Lien->id_pipe);

    mkfifo(Lien->pipe_request, 0666);
    mkfifo(Lien->pipe_response, 0666);
}

void publier_id_pipe(LienCommunication *Lien, const char *handshake_path)
{
    mkfifo(handshake_path, 0666);
    int fd = open(handshake_path, O_WRONLY);
    write(fd, &Lien->id_pipe, sizeof(int));
    close(fd);
}

void rejoindre_pipe(LienCommunication *Lien, const char *source, const char *handshake_path)
{
    int fd = open(handshake_path, O_RDONLY);
    read(fd, &Lien->id_pipe, sizeof(int));
    close(fd);

    snprintf(Lien->pipe_request, 256, "/tmp/%s_%d_req.fifo", source, Lien->id_pipe);
    snprintf(Lien->pipe_response, 256, "/tmp/%s_%d_res.fifo", source, Lien->id_pipe);
}

int parser_requete(const char *requete_str, RequeteQR *requete) {
    return sscanf(requete_str, "|%d|%d|%d|", &requete->code_serveur, &requete->code_lieu, &requete->code_menu); 
}

int valider_format_requete(const char *requete_str){
    RequeteQR requete;
    int pars = parser_requete(requete_str, &requete);
    if(pars != 3){
        fprintf(stderr, "Erreur : format invalide\n");
        return 0;
    }
    return 1;

}