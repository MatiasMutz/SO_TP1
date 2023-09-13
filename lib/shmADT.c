#include "shmADT.h"

#define FILE_SIZE 128

typedef struct shmCDT {
    sem_t sem1;
    sem_t sem2;
    size_t size;
    char* buffer;
} shmCDT;

shmADT createShM(int filesQty) {
    shmADT shm = malloc(sizeof(shmCDT));

    if (shm == NULL) {
        perror("Error in malloc");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open("/shm", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

    if (shm_fd == -1) {
        perror("Error in shm_open");
        free(shm);
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, filesQty * FILE_SIZE) == -1) {
        perror("Error in ftruncate");
        free(shm);
        exit(EXIT_FAILURE);
    }

    char* ptr = mmap(NULL, filesQty * FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED) {
        perror("Error in mmap");
        free(shm);
        exit(EXIT_FAILURE);
    }

    // EL MANUAL DE MMAP DICE ESTO:
    // After the mmap() call has returned, the file descriptor, fd, can be closed immediately without invalidating the mapping.

    if(shm_unlink("/shm") == -1){
        perror("Error in shm_unlink");
        free(shm);
        exit(EXIT_FAILURE);
    }

    shm->size = filesQty * FILE_SIZE;
    shm->buffer = ptr;
}