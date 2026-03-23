#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    char message_relais[256];
    char *pipe_relais = "/tmp/pipe_relais";

    char *pipe_communication = "/tmp/pipe_communication";

    mkfifo(pipe_communication, 0666);

    int fd = open(pipe_communication, O_RDONLY);
    int fd2 = open(pipe_relais, O_WRONLY);
    if (fd2 == -1)
    {
        perror("Erreur: Le pipe relais n'existe pas encore!");
        return 1;
    }
    printf("En attente d'un message à relayer.\n");

    read(fd, message_relais, sizeof(message_relais));
    printf("Le lecteur a recu: %s\n", message_relais);

    write(fd2, message_relais, sizeof(message_relais));
    printf("Message envoyé vers le relais");

    close(fd);
    unlink(pipe_communication);

    return 0;
}