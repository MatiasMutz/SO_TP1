#include "include/app.h"

typedef struct {
    int number;
    int filesProcessed;
    pid_t pid;
} slave;


void cleanPath(char *path) {
    for (int i = 0; path[i]; i++) {
        path[i] = '\0';
    }
}

void checkRealloc(const char *s) {
    if (s == NULL) {
        perror("Error in realloc");
        exit(EXIT_FAILURE);
    }
}

void sendToSlave(slave slave[], int fdsWrite[][2], const char *path, int *filesRemaining, int *count, int slaveNumber) {
    char *toSend = NULL;
    size_t len;
    for (len = 0; path[len]; len++) {
        if (len % BLOQUE == 0) {
            toSend = realloc(toSend, sizeof(char) * (len + BLOQUE));
            checkRealloc(toSend);
        }
        toSend[len] = path[len];
    }
    toSend = realloc(toSend, len + 1);
    checkRealloc(toSend);
    toSend[len] = ' ';
    write(fdsWrite[slaveNumber][STDOUT_FILENO], toSend, len + 1);
    free(toSend);
    (*filesRemaining)--;
    (*count)++;
    slave[slaveNumber].filesProcessed++;
}

int main(int argc, char *argv[]) {
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
            close(fdsWrite[i][STDIN_FILENO]);
            close(fdsRead[i][STDOUT_FILENO]);
        } else {
            // HIJO
            close(STDIN_FILENO);
            dup(fdsWrite[i][STDIN_FILENO]);
            close(fdsWrite[i][STDIN_FILENO]);
            close(fdsWrite[i][STDOUT_FILENO]);

            close(STDOUT_FILENO);
            dup(fdsRead[i][STDOUT_FILENO]);
            close(fdsRead[i][STDIN_FILENO]);
            close(fdsRead[i][STDOUT_FILENO]);
            execve("slave", (char *[]) {NULL}, (char *[]) {NULL});
            perror("Error in execve");
            exit(EXIT_FAILURE);
        }
    }

    char buffer[256] = {'\0'};
    int charsRead;
    int retval;
    fd_set rdfs;
    int maxFd;

    int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);

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
            perror("Error in select");
            exit(EXIT_FAILURE);
        } else if (retval) {
            for (int i = 0; i < slavesQty && filesRemaining > 0; i++) {
                if (FD_ISSET(fdsRead[i][STDIN_FILENO], &rdfs)) {
                    // si elimino un fd del set tengo que volver a calcular el maximo

                    charsRead = read(fdsRead[i][STDIN_FILENO], buffer, 256);
                    buffer[charsRead] = '\0';
                    dprintf(fd, "PID: %d %s", slaves[i].pid, buffer);

                    char aux[256] = {'\0'};
                    for (int j = 0; j < charsRead; j++) {
                        aux[j] = buffer[j];
                        if (buffer[j] == '\n') {
                            aux[j] = '\n';
                            dprintf(fd, "PID: %d %s", slaves[i].pid, aux);
                            cleanPath(aux);
                        }
                    }

                    if (slaves[i].filesProcessed >= INITIAL_LOAD) {
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