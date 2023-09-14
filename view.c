#include "lib/view.h"

#define PATH_SIZE 128

int main(int argc, char *argv[]) {
    char shmpath[PATH_SIZE] = {0};
    int filesQty;

    if (argc == 3) {  // recibio el nombre de la shm por linea de comandos y la cantidad de archivos
        strcpy(shmpath, argv[1]);
        filesQty = atoi(argv[2]);
    } else if (argc == 1) {  // recibio el nombre de la shm por STDIN
        int charsRead = read(STDIN_FILENO, shmpath, PATH_SIZE);
        if (charsRead == -1) {
            perror("Error in read");
            exit(EXIT_FAILURE);
        }
        char aux[10];
        int start;
        for (int i = 0; i < charsRead; i++) {
            if (shmpath[i] == ' ') {
                start = i + 1;
            }
        }

        for (int i = start, j = 0; i < charsRead; i++, j++) {
            aux[j] = shmpath[i];
        }
        filesQty = atoi(aux);

    } else {
        perror("Error: Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    shmADT shm = connect_shm(shmpath);

    char buffer[BUFSIZ];

    printf("%d\n", filesQty);

    while (filesQty > 0) {
        read_shm(shm, buffer);
        printf("%s\n", buffer);
        filesQty--;
    }
    return 0;
}