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

shmADT create_shm(const char *shmpath) {
    shmADT shm;

    //shm_unlink(shmpath);
    int shm_fd = shm_open(shmpath, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

    if (shm_fd == -1) {
        perror("Error in shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shmCDT)) == -1) {
        perror("Error in ftruncate");
        exit(EXIT_FAILURE);
    }

    shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm == MAP_FAILED) {
        perror("Error in mmap");
        // shm_unlink(shmpath); todo
        // close(shm_fd); todo
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shm->hasData, 1, 0) == -1) {
        perror("Error in sem_init");
        // shm_unlink(shmpath); todo
        // close(shm_fd); todo
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

shmADT connect_shm(char *shmpath) {
    shmADT shm;

    int shm_fd = shm_open(shmpath, O_RDWR, S_IRUSR | S_IWUSR);

    if (shm_fd == -1) {
        perror("Error in shm_open");
        exit(EXIT_FAILURE);
    }

    shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm == MAP_FAILED) {
        perror("Error in mmap");
        exit(EXIT_FAILURE);
    }

    return shm;
}

void write_shm(shmADT shm, char *input, size_t size) {
    if (input == NULL) {
        perror("Error in buffer");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < size; i++, (shm->wIndex)++) {
        shm->buffer[shm->wIndex] = input[i];
    }
    sem_post(&shm->hasData);
}

int read_shm(shmADT shm, char *output) {
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

void close_shm(shmADT shm) {
    if (munmap(shm, sizeof(*shm)) == -1) {
        perror("Error in munmap");
        exit(EXIT_FAILURE);
    }
}

void close_shm_connection(shmADT shm) {
    char aux[PATH_SIZE];
    strcpy(aux, shm->path);
    if (sem_destroy(&shm->hasData) == -1) {
        perror("Error in sem_destroy");
        exit(EXIT_FAILURE);
    }
    close_shm(shm);
    if (shm_unlink(aux) == -1) {
        perror("Error in shm_unlink");
        exit(EXIT_FAILURE);
    }
}
