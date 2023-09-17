// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "shmADT.h"

#define PATH_SIZE 256

typedef struct shmCDT {
    char path[PATH_SIZE];
    sem_t hasData;
    unsigned int wIndex, rIndex;
    char buffer[BUFSIZ];
} shmCDT;

shmADT shmCreate(const char *shmpath) {
    shmADT shm;

    //shm_unlink(shmpath);
    int shmFd = shm_open(shmpath, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

    if (shmFd == -1) {
        perror("Error in shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shmFd, sizeof(shmCDT)) == -1) {
        perror("Error in ftruncate");
        exit(EXIT_FAILURE);
    }

    shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    if (shm == MAP_FAILED) {
        perror("Error in mmap");
        // shm_unlink(shmpath); todo
        // close(shmFd); todo
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shm->hasData, 1, 0) == -1) {
        perror("Error in sem_init");
        // shm_unlink(shmpath); todo
        // close(shmFd); todo
        exit(EXIT_FAILURE);
    }

    int i;

    for (i = 0; shmpath[i]; i++) {
        shm->path[i] = shmpath[i];
    }

    shm->path[i] = 0;

    shm->wIndex = 0;
    shm->rIndex = 0;

    return shm;
}

shmADT shmConnect(char *shmpath) {
    shmADT shm;

    int shmFd = shm_open(shmpath, O_RDWR, S_IRUSR | S_IWUSR);

    if (shmFd == -1) {
        perror("Error in shm_open");
        exit(EXIT_FAILURE);
    }

    shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    if (shm == MAP_FAILED) {
        perror("Error in mmap");
        exit(EXIT_FAILURE);
    }

    return shm;
}

void shmWrite(shmADT shm, char *input, size_t size) {
    if (input == NULL) {
        perror("Error in buffer");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < size; i++, (shm->wIndex)++) {
        shm->buffer[shm->wIndex] = input[i];
    }
    sem_post(&shm->hasData);
}

int shmRead(shmADT shm, char *output) {
    sem_wait(&shm->hasData);
    int i;

    if(shm->buffer[shm->rIndex] == '\0')
        return -1;

    for (i = 0; shm->buffer[shm->rIndex] != '\n'; i++, (shm->rIndex)++) {
        output[i] = shm->buffer[shm->rIndex];
    }
    output[i] = '\n';
    output[i + 1] = 0;
    (shm->rIndex)++;

    return 0;
}

void shmClose(shmADT shm) {
    if (munmap(shm, sizeof(*shm)) == -1) {
        perror("Error in munmap");
        exit(EXIT_FAILURE);
    }
}

void shmCloseConnection(shmADT shm) {
    char aux[PATH_SIZE];
    strcpy(aux, shm->path);
    if (sem_destroy(&shm->hasData) == -1) {
        perror("Error in sem_destroy");
        exit(EXIT_FAILURE);
    }
    shmClose(shm);
    if (shm_unlink(aux) == -1) {
        perror("Error in shm_unlink");
        exit(EXIT_FAILURE);
    }
}
