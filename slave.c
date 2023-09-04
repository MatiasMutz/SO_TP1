#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *argv_md5sum[] = {"md5sum", argv[1], NULL};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    execve("/usr/bin/md5sum", argv_md5sum, NULL);
    
    perror("execve"); /* execve() returns only on error */
    exit(EXIT_FAILURE);
}