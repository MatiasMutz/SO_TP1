#include "include/app.h"
#include <math.h>

int main(int argc, char *argv[]) {
    int filesQty = argc - 1;

    int slavesQty = (int) ceil(((double) filesQty / FILES_PER_SLAVE));

    int status;
    pid_t pid;

    int fds[slavesQty][2]; // Array de pipes

    int count = 1;

    printf("Cantidad de archivos: %d\nCantidad de esclavos: %d\n", filesQty, slavesQty);
    for (int i = 0; i < slavesQty; i++) {
        if (pipe(fds[i]) < 0) { // Creo un pipe, antes del fork asi ambos procesos lo tienen
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork()) != 0) {
            // PADRE
            close(1); // Cerramos STDOUT en padre
            dup(fds[i][1]); // Dupeamos write end of pipe -> Lo manda a STDOUT
            for (int j = 0; j < INITIAL_LOAD && count <= filesQty; j++, count++) {
                char* aux = malloc(strlen(argv[count]) + 1);
                strcpy(aux, argv[count]);
                aux[strlen(argv[count])] = ' ';
                write(1, aux, strlen(aux));
                free(aux);
            }
            close(fds[i][0]); // Cerramos ambos extremos
            close(fds[i][1]);
        } else {
            // HIJO
            printf("\nHijo %d\n", i);
            close(0); // Cerramos STDIN en padre
            dup(fds[i][0]); // Dupeamos read end of pipe -> Lo manda a STDIN
            close(fds[i][0]); // Cerrramos ambos extremos del pipe
            close(fds[i][1]);
            execve("slave", NULL, NULL);
        }
    }
    //while (waitpid(-1, &status, 0) > 0);
}