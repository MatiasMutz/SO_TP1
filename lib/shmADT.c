#include "shmADT.h"

typedef struct shmCDT {
    sem_t sem1;
    sem_t sem2;
    size_t size;
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
        exit(EXIT_FAILURE);
    }

    shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm == MAP_FAILED) {
        perror("Error in mmap");
        exit(EXIT_FAILURE);
    }

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

int write_shm(shmADT shm, char *buffer, size_t size) { return 0; }

int read_shm(shmADT shm, char *buffer, size_t size) { return 0; }

int close_shm(shmADT shm) { return 0; }