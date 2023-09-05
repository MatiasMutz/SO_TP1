#include "include/slave.h"

int main(int argc, char *argv[]) {
    printf("\nSoy el hijo %d\n", getpid());
    char* buffer = malloc(256);
    read(0, buffer, 256);
    printf("%s\n", buffer);
    free(buffer);
    return 0;

    /*
    char *argv_md5sum[] = {"md5sum", "app.c", "slave.c", NULL};

    execve("/usr/bin/md5sum", argv_md5sum, NULL);

    perror("execve"); // execve() returns only on error
    exit(EXIT_FAILURE);
     */
}