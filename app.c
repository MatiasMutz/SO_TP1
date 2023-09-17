// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib/app.h"

#define FILES_PER_SLAVE 20
#define INITIAL_LOAD 5
#define BLOQUE 5
#define VIEW_TIMEOUT 2
#define INFO_SIZE 256
#define SLAVE_SIZE (INFO_SIZE + 16)

typedef struct slave {
    int filesProcessed;
    pid_t pid;
} slave;

static void sendToSlave(int fdsAppToSlave[][2], const char *path, int *count, int slaveNumber);
static void * doRealloc(void *toSend, size_t len);

int main(int argc, char *argv[]) {
    int filesQty = argc - 1;
    int filesRemaining = filesQty;

    int slavesQty = (int)ceil(((double)filesQty / FILES_PER_SLAVE));

    pid_t pid;

    // disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    int fdsAppToSlave[slavesQty][2];  // Array de pipes de escritura (app -> slave)
    int fdsSlaveToApp[slavesQty][2];  // Array de pipes de lectura (slave -> app)

    slave slaves[slavesQty];

    int count = 1;

    shmADT shm = shmCreate("/shm");

    printf("/shm\n");
    sleep(VIEW_TIMEOUT);

    for (int i = 0; i < slavesQty; i++) {
        slaves[i].filesProcessed = 0;
        if (pipe(fdsAppToSlave[i]) < 0 || pipe(fdsSlaveToApp[i]) < 0) {
            perror("pipe creation error");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork()) != 0) {
            // PADRE
            slaves[i].pid = pid;
            for (int j = 0; j < INITIAL_LOAD && count <= filesQty; j++) {
                sendToSlave(fdsAppToSlave, argv[count], &count, i);
            }
            close(fdsAppToSlave[i][STDIN_FILENO]);
            close(fdsSlaveToApp[i][STDOUT_FILENO]);
        } else {
            // HIJO
            close(STDIN_FILENO);
            dup(fdsAppToSlave[i][STDIN_FILENO]);
            close(fdsAppToSlave[i][STDIN_FILENO]);
            close(fdsAppToSlave[i][STDOUT_FILENO]);

            close(STDOUT_FILENO);
            dup(fdsSlaveToApp[i][STDOUT_FILENO]);
            close(fdsSlaveToApp[i][STDIN_FILENO]);
            close(fdsSlaveToApp[i][STDOUT_FILENO]);
            execve("slave", (char *[]){"slave", NULL}, (char *[]){NULL});
            perror("Error in execve");
            exit(EXIT_FAILURE);
        }
    }

    char buffer[INFO_SIZE];
    char slaveData[SLAVE_SIZE];
    int charsRead, retval, maxFd;
    fd_set rdfs;
    size_t lenAux;
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(fd == -1){
        perror("Error in open");
        exit(EXIT_FAILURE);
    }

    while (filesRemaining > 0) {
        FD_ZERO(&rdfs);  // elimina todos los fd, vacia el array
        maxFd = 0;

        for (int i = 0; i < slavesQty; i++) {
            // para monitorear cuando hay info disponible en stdin. Agrega el fd al array
            FD_SET(fdsSlaveToApp[i][STDIN_FILENO], &rdfs);
            if (fdsSlaveToApp[i][STDIN_FILENO] > maxFd)
                maxFd = fdsSlaveToApp[i][STDIN_FILENO];
        }

        retval = select(maxFd + 1, &rdfs, NULL, NULL, NULL);  // espera a que haya info disponible en stdin

        if (retval == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < slavesQty && filesRemaining > 0; i++) {
            if (FD_ISSET(fdsSlaveToApp[i][STDIN_FILENO], &rdfs)) {
                // si elimino un fd del set tengo que volver a calcular el maximo

                charsRead = read(fdsSlaveToApp[i][STDIN_FILENO], buffer, INFO_SIZE);

                for (int j = 0; j < charsRead && j < INFO_SIZE - 1; j++) {
                    if (buffer[j] == '\n') {
                        buffer[++j] = '\0';

                        filesRemaining--;

                        sprintf(slaveData, "PID %d %s", slaves[i].pid, buffer);

                        lenAux = strlen(slaveData);
                        write(fd, slaveData, lenAux);
                        shmWrite(shm, slaveData, lenAux);

                        slaves[i].filesProcessed++;
                    }
                }

                if (slaves[i].filesProcessed >= INITIAL_LOAD && filesRemaining > 0 && count <= filesQty)
                    sendToSlave(fdsAppToSlave, argv[count], &count, i);
            }
        }
    }
    shmWrite(shm, "\0", 1);
    
    for (int i = 0; i < slavesQty; i++) {
        close(fdsSlaveToApp[i][STDIN_FILENO]);
        close(fdsAppToSlave[i][STDOUT_FILENO]);
    }
    shmClose(shm);
    close(fd);
    return 0;
}

void sendToSlave(int fdsAppToSlave[][2], const char *path, int *count, int slaveNumber) {
    char *toSend = NULL;
    size_t len;
    for (len = 0; path[len]; len++) {
        if (len % BLOQUE == 0) {
            toSend = doRealloc(toSend, sizeof(char) * (len + BLOQUE));
        }
        toSend[len] = path[len];
    }

    toSend = doRealloc(toSend, len + 1);

    toSend[len] = ' ';
    write(fdsAppToSlave[slaveNumber][STDOUT_FILENO], toSend, len + 1);
    free(toSend);
    (*count)++;
}
void * doRealloc(void *toSend, size_t len) {
    void *aux = realloc(toSend, len);
    if (aux == NULL) {
        free(toSend);
        perror("Error in realloc");
        exit(EXIT_FAILURE);
    }
    return aux;
}