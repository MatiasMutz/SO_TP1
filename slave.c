#include "include/slave.h"

int main(int argc, char *argv[]) {
    
/*     printf("\nSoy el hijo %d\n", getpid());
    char* buffer = malloc(256);
    read(STDIN_FILENO, buffer, 256);
    printf("%s\n", buffer);
    free(buffer);
    return 0; */

    char *arguments[] = {"md5sum", "app.c", "slave.c", NULL};
    
    execlp("md5sum", arguments, (char *) NULL);

    perror("execlp"); // execlp() returns only on error
    exit(EXIT_FAILURE);
}