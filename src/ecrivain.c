#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char message_utilisateur[256];
    char *pipe_communication = "/tmp/pipe_communication";

    fd = open(pipe_communication, O_WRONLY);

    if (fd == -1)
    {
        perror("Erreur: Le pipe de communication n'existe pas encore!");
        return 1;
    }

    printf("Entrez un message à envoyer au lecteur : ");
    fgets(message_utilisateur, sizeof(message_utilisateur), stdin);
    write(fd, message_utilisateur, sizeof(message_utilisateur));

    printf("Écrivain : Message envoyé.\n");

    close(fd);

    return 0;
}