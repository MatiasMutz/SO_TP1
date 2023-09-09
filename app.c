#include "include/app.h"
#include <math.h>

int main(int argc, char *argv[]) {
    int filesQty = argc - 1;
    int filesRemaining = filesQty;

    int slavesQty = (int) ceil(((double) filesQty / FILES_PER_SLAVE));

    int status;
    pid_t pid;

    int fdsWrite[slavesQty][2]; // Array de pipes de escritura
    int fdsRead[slavesQty][2]; // Array de pipes de lectura

    int count = 1;

    printf("Cantidad de archivos: %d\nCantidad de esclavos: %d\n", filesQty, slavesQty);

    for (int i = 0; i < slavesQty; i++) {
        if (pipe(fdsWrite[i]) < 0 || pipe(fdsRead[i]) < 0) { // Creo un pipe, antes del fork asi ambos procesos lo tienen
            perror("pipe creation error");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork()) != 0) {
            // PADRE
            close(STDOUT_FILENO); // Cerramos STDOUT en padre
            dup(fdsWrite[i][STDOUT_FILENO]); // Dupeamos write end of pipe -> Lo manda a STDOUT

            close(STDIN_FILENO); // Cerramos STDIN en padre
            dup(fdsRead[i][STDIN_FILENO]); // Dupeamos read end of pipe -> Lo manda a STDIN


            for (int j = 0; j < INITIAL_LOAD && count <= filesQty; j++, count++) {
                int length = strlen(argv[count]);
                char aux[length + 1]; //todo estaba bien declarar array no estático con tamaño variable o hay que usar malloc y free?
                strcpy(aux, argv[count]);
                aux[length] = ' ';
                write(fdsWrite[i][STDOUT_FILENO], aux, length + 1);
                filesRemaining--;
            }

            close(fdsWrite[i][STDIN_FILENO]); // Cerramos ambos extremos
            close(fdsWrite[i][STDOUT_FILENO]);

            close(fdsRead[i][STDIN_FILENO]); // Cerramos ambos extremos
            close(fdsRead[i][STDOUT_FILENO]);
        } else {
            // HIJO
            close(STDIN_FILENO); // Cerramos STDIN en padre
            dup(fdsWrite[i][STDIN_FILENO]); // Dupeamos read end of pipe -> Lo manda a STDIN
            close(fdsWrite[i][STDIN_FILENO]); // Cerrramos ambos extremos del pipe
            close(fdsWrite[i][STDOUT_FILENO]);

            close(STDOUT_FILENO); // Cerramos STDOUT en padre
            dup(fdsRead[i][STDOUT_FILENO]); // Dupeamos write end of pipe -> Lo manda a STDOUT
            close(fdsRead[i][STDIN_FILENO]); // Cerrramos ambos extremos del pipe
            close(fdsRead[i][STDOUT_FILENO]);
            execve("slave", NULL, NULL);
        }
        waitpid(pid, &status, 0);
    }

    fd_set readfds;

    while(filesRemaining > 0){
        FD_ZERO(&readfds);
        for(int i = 0; i < slavesQty; i++){
            FD_SET(fdsRead[i][STDOUT_FILENO], &readfds);
            filesRemaining--;
        }
    }
    return 0;
}