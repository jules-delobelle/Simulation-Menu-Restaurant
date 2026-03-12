#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    int fd2;
    char *pipe_relais = "/tmp/pipe_relais";
    char message_final[256];

    mkfifo(pipe_relais, 0666);

    fd2 = open(pipe_relais, O_RDONLY);

    printf("En attente du message relayé.\n");
    read(fd2, message_final, sizeof(message_final));
    printf("Le lecteur final a recu: %s\n", message_final);

    close(fd2);
    unlink(pipe_relais);

    return 0;
}