#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    int fd;
    char *pipe_communication = "/tmp/pipe_communication";
    char tampon[100];

    mkfifo(pipe_communication, 0666);

    fd = open(pipe_communication, O_RDONLY);

    read(fd, tampon, sizeof(tampon));
    printf("Le lecteur a recu: %s\n", tampon);

    close(fd);
    unlink(pipe_communication);

    return 0;
}