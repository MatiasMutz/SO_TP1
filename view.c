#include "lib/view.h"

#define PIPE_INFO 128
#define PATH_SIZE 64
#define NUMBER_SIZE 16

int main(int argc, char *argv[]) {
    char shmPath[PATH_SIZE];
    int filesQty;

    if (argc == 3) {  // recibio el nombre de la shm por linea de comandos y la cantidad de archivos
        strcpy(shmPath, argv[1]);
        filesQty = atoi(argv[2]);

    } else if (argc == 1) {  // recibio el nombre de la shm por STDIN
        char *aux = NULL;
        char number[NUMBER_SIZE];
        size_t infoSize = 0;
        int delimiterPos;

        if (getline(&aux, &infoSize, stdin) == -1) {
            free(aux);
            perror("Error in getline");
            exit(EXIT_FAILURE);
        }

        for (delimiterPos = 0; aux[delimiterPos] != ' '; delimiterPos++) {
            shmPath[delimiterPos] = aux[delimiterPos];
        }

        shmPath[delimiterPos] = 0;

        int j = 0;

        for (size_t i = delimiterPos + 1; aux[i] != '\n'; i++, j++) {
            number[j] = aux[i];
        }

        number[j] = 0;

        filesQty = atoi(number);

        free(aux);

    } else {
        perror("Error: Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    shmADT shm = connect_shm(shmPath);

    char buffer[BUFSIZ];

    while (filesQty > 0) {
        read_shm(shm, buffer);
        printf("%s", buffer);
        filesQty--;
    }

    // TODO: liberar memoria compartida
    close_shm_connection(shm);
    return 0;
}