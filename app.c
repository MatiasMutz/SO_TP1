#include "include/app.h"
#include <math.h>

void sendToSlave(int slaveNumber, int fdsWrite[][2], char *path, int *filesRemaining, int *count) {
    int length = strlen(path);
    //todo estaba bien declarar array no estático con tamaño variable o hay que usar malloc y free?
    char aux[length + 1];
    strcpy(aux, path);
    aux[length] = ' ';
    write(STDOUT_FILENO, aux, length + 1);
    (*filesRemaining)--;
    (*count)++;
}


int main(int argc, char *argv[]) {
    char *newargv[] = {NULL};
    char *newenv[] = {NULL};
    int filesQty = argc - 1;
    int filesRemaining = filesQty;

    //int slavesQty = (int) ceil(((double) filesQty / FILES_PER_SLAVE));
    int slavesQty = 2;

    int status;
    pid_t pid;

    int fdsWrite[slavesQty][2]; // Array de pipes de escritura
    int fdsRead[slavesQty][2]; // Array de pipes de lectura

    int count = 1;

    printf("Cantidad de archivos: %d\nCantidad de esclavos: %d\n", filesQty, slavesQty);

    for (int i = 0; i < slavesQty; i++) {
        if (pipe(fdsWrite[i]) < 0 ||
            pipe(fdsRead[i]) < 0) { // Creo un pipe, antes del fork asi ambos procesos lo tienen
            perror("pipe creation error");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork()) != 0) {
            // PADRE
            close(STDOUT_FILENO); // Cerramos STDOUT en padre
            dup(fdsWrite[i][STDOUT_FILENO]); // Dupeamos write end of pipe -> Lo manda a STDOUT

            close(STDIN_FILENO); // Cerramos STDIN en padre
            dup(fdsRead[i][STDIN_FILENO]); // Dupeamos read end of pipe -> Lo manda a STDIN


            for (int j = 0; j < INITIAL_LOAD && count <= filesQty; j++) {
                sendToSlave(i, fdsWrite, argv[count], &filesRemaining, &count);
            }

            //close(fdsWrite[i][STDIN_FILENO]); // Cerramos ambos extremos
            //close(fdsWrite[i][STDOUT_FILENO]);

            //close(fdsRead[i][STDIN_FILENO]); // Cerramos ambos extremos
            //close(fdsRead[i][STDOUT_FILENO]);
        } else {
            // HIJO
            close(STDIN_FILENO); // Cerramos STDIN en padre
            dup(fdsWrite[i][STDIN_FILENO]); // Dupeamos read end of pipe -> Lo manda a STDIN
            //close(fdsWrite[i][STDIN_FILENO]); // Cerrramos ambos extremos del pipe
            //close(fdsWrite[i][STDOUT_FILENO]);

            close(STDOUT_FILENO); // Cerramos STDOUT en padre
            dup(fdsRead[i][STDOUT_FILENO]); // Dupeamos write end of pipe -> Lo manda a STDOUT
            //close(fdsRead[i][STDIN_FILENO]); // Cerrramos ambos extremos del pipe
            //close(fdsRead[i][STDOUT_FILENO]);
            execve("slave", newargv, newenv);
        }
        waitpid(pid, &status, 0);
    }

    int retval;
    fd_set rdfs;
    int maxFd;

    while (filesRemaining > 0) {
        FD_ZERO(&rdfs); //elimina todos los fd, vacia el array
        maxFd = 0;
        for (int i = 0; i < slavesQty; i++) {
            // para monitorear cuando hay info disponible en stdin. Agrega el fd al array
            FD_SET(fdsRead[i][STDIN_FILENO], &rdfs);
            if (fdsRead[i][STDIN_FILENO] > maxFd)
                maxFd = fdsRead[i][STDIN_FILENO];
        }
        retval = select(maxFd + 1, &rdfs, NULL, NULL, NULL); // espera a que haya info disponible en stdin
        if (retval == -1) {
            perror("select()");
            exit(EXIT_FAILURE);
        } else if (retval) {
            for (int i = 0; i <= maxFd; i++) {
                if (FD_ISSET(fdsRead[i][STDIN_FILENO], &rdfs)) {
                    // si elimino un fd del set tengo que volver a calcular el maximo
                    //int fp = open("resultado.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);

                    if (filesRemaining > 0) {
                        sendToSlave(i, fdsWrite, argv[count], &filesRemaining, &count);
                    }
                }
            }
        }
    }
    for (int i = 0; i < slavesQty; i++) {
        close(fdsRead[i][STDIN_FILENO]);
        close(fdsRead[i][STDOUT_FILENO]);
        close(fdsWrite[i][STDIN_FILENO]);
        close(fdsWrite[i][STDOUT_FILENO]);
    }
    return 0;
}