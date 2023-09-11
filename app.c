#include "include/app.h"
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct {
    int number;
    int filesProcessed;
    pid_t pid;
} slave;

//void sendToSlave(int slaveNumber, int fdsWrite[][2], char *path, int *filesRemaining, int *count) {
void sendToSlave(slave slave[], int fdsWrite[][2], char *path, int *filesRemaining, int *count, int slaveNumber) {
    int length = strlen(path);
    //todo estaba bien declarar array no estático con tamaño variable o hay que usar malloc y free?
    char aux[length + 1];
    strcpy(aux, path);
    aux[length] = ' ';
    write(fdsWrite[slaveNumber][STDOUT_FILENO], aux, length + 1);
    (*filesRemaining)--;
    (*count)++;
    slave[slaveNumber].filesProcessed++;
}


int main(int argc, char *argv[]) {
    char *newargv[] = {NULL};
    char *newenv[] = {NULL};
    int filesQty = argc - 1;
    int filesRemaining = filesQty;

    int slavesQty = (int) ceil(((double) filesQty / FILES_PER_SLAVE));

    int status;
    pid_t pid;

    //disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    int fdsWrite[slavesQty][2]; // Array de pipes de escritura
    int fdsRead[slavesQty][2]; // Array de pipes de lectura

    slave slaves[slavesQty];

    int count = 1;


    printf("Cantidad de archivos: %d\nCantidad de esclavos: %d\n", filesQty, slavesQty);

    for (int i = 0; i < slavesQty; i++) {
        slaves[i].number = i;
        slaves[i].filesProcessed = 0;
        if (pipe(fdsWrite[i]) < 0 || pipe(fdsRead[i]) < 0) {
            // Creo un pipe, antes del fork asi ambos procesos lo tienen
            perror("pipe creation error");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork()) != 0) {
            // PADRE
            slaves[i].pid = pid;
            for (int j = 0; j < INITIAL_LOAD && count <= filesQty; j++) {
               // sendToSlave(i, fdsWrite, argv[count], &filesRemaining, &count);
               sendToSlave(slaves, fdsWrite, argv[count], &filesRemaining, &count, i);
            }
            close(fdsWrite[i][0]);

            close(fdsRead[i][1]);
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
            execve("slave", newargv, newenv);
            perror("Error in execve.");
            exit(EXIT_FAILURE);
        }
    }

    int retval;
    fd_set rdfs;
    int maxFd;
    char buffer[256] = {'\0'};
    int charsRead;

    int fd = open("output.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);

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
            for (int i = 0; i < slavesQty; i++) {
                if (FD_ISSET(fdsRead[i][STDIN_FILENO], &rdfs)) {
                    // si elimino un fd del set tengo que volver a calcular el maximo
                    charsRead = read(fdsRead[i][STDIN_FILENO], buffer, 256);
                    buffer[charsRead] = '\0';
                    dprintf(fd, "PID: %d %s", slaves[i].pid, buffer);

                    if (filesRemaining > 0 && slaves[i].filesProcessed >= 5) {//aca hay que chequear que el esclavo ya haya terminado de procesar los primeros 5 archivos.
                        sendToSlave(slaves, fdsWrite, argv[count], &filesRemaining, &count, i);
                    }
                }
            }
        }
    }

    for (int i = 0; i < slavesQty; i++) {
        close(fdsRead[i][0]);
        close(fdsWrite[i][1]);

    }
    close(fd);
    return 0;
}