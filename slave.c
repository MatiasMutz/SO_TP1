#include "include/slave.h"

void limpiarPath(char *path) {
    for (int i = 0; path[i]; i++) {
        path[i] = '\0';
    }
}

int main(int argc, char *argv[]) {
    char *buffer = malloc(256);
    read(STDIN_FILENO, buffer, 256);
    printf("%s\n", buffer);

    char *path = malloc(256);

    for (int i = 0, j = 0; buffer[j]; i++, j++) {
        if (buffer[j] == ' ') {
            printf("Path: %s\n", path);
            limpiarPath(path);
            /*if (fork() == 0) {
                execlp("md5sum", "md5sum", path, NULL);
            }*/
            i = -1;
        } else {
            path[i] = buffer[j];
        }
    }
    free(buffer);
    free(path);
    return 0;

    /*
    char *arguments[] = {"md5sum", "app.c", "slave.c", NULL};

    execlp("md5sum", arguments, (char *) NULL);

    perror("execlp"); // execlp() returns only on error
    exit(EXIT_FAILURE);
     */
}