#include "shmADT.h"

typedef struct shmCDT {
    char path[64];
    sem_t hasData;
    unsigned int wIndex, rIndex;
    char buffer[BUFSIZ];
} shmCDT;

shmADT create_shm(const char *shmpath) {
    shmADT shm;

    int shm_fd = shm_open(shmpath, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

    if (shm_fd == -1) {
        perror("Error in shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shmCDT)) == -1) {
        perror("Error in ftruncate");
        // shm_unlink(shmpath); todo
        // close(shm_fd); todo
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

    strcpy(shm->path, shmpath);
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

void write_shm(shmADT shm, char *result, size_t size) {
    if (result == NULL) {
        perror("Error in buffer");
        exit(EXIT_FAILURE);
    }
    for (int i = shm->wIndex; i < shm->wIndex + size; i++) {
        shm->buffer[i] = result[i];
    }
    sem_post(&shm->hasData);
}

void read_shm(shmADT shm, char *output) {
    sem_wait(&shm->hasData);
    for (int i = shm->rIndex; output[i] != '\n'; i++) {
        output[i] = shm->buffer[i];
    }
}

void close_shm(shmADT shm) {
    if (sem_destroy(&shm->hasData) == -1) {
        perror("Error in sem_destroy");
        exit(EXIT_FAILURE);
    }

    if (munmap(shm, sizeof(*shm)) == -1) {
        perror("Error in munmap");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(shm->path) == -1) {
        perror("Error in shm_unlink");
        exit(EXIT_FAILURE);
    }
}