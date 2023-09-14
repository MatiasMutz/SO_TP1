#include "lib/view.h"

#define PATH_SIZE 128

int main(int argc, char *argv[]) {
    char shmpath[PATH_SIZE] = {0};

    if (argc == 2)  // recibio el nombre de la shm por linea de comandos
        strcpy(shmpath, argv[1]);

    else if (argc == 1) {  // recibio el nombre de la shm por STDIN
        int charsRead = read(STDIN_FILENO, shmpath, PATH_SIZE);

        if (charsRead == -1) {
            perror("Error in read");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("Error: Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    shmADT shm = connect_shm(shmpath);

    char *buffer = malloc(sizeof(char) * MAX_LEN);
    if (buffer == NULL) {
        perror("Error in malloc");
        exit(EXIT_FAILURE);
    }




    free(buffer);
}