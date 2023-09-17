// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib/view.h"

#define PIPE_INFO 128
#define PATH_SIZE 64
#define NUMBER_SIZE 16
#define END_READ -1

int main(int argc, char *argv[]) {
    char shmPath[PATH_SIZE];

    if (argc == 2) {  // recibio el nombre de la shm por linea de comandos
        strncpy(shmPath, argv[1], PATH_SIZE - 1);

    } else if (argc == 1) {  // recibio el nombre de la shm por STDIN
        char *aux = NULL;
        size_t infoSize = 0;
        int i = 0;

        if (getline(&aux, &infoSize, stdin) == -1) {
            free(aux);
            perror("Error in getline");
            exit(EXIT_FAILURE);
        }

        for (i = 0; aux[i] != '\n'; i++) {
            shmPath[i] = aux[i];
        }

        shmPath[i] = 0;

        free(aux);

    } else {
        perror("Error: Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    shmADT shm = connect_shm(shmPath);

    char buffer[BUFSIZ];

    while (read_shm(shm, buffer) != END_READ) {
        printf("%s", buffer);
    }

    close_shm_connection(shm);
    return 0;
}