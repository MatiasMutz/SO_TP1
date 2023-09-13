#ifndef _SHMADT_H
#define _SHMADT_H

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct shmCDT *shmADT;

shmADT createShM(int filesQty);

int writeShM(shmADT shm, char *buffer, size_t size);

int readShM(shmADT shm, char *buffer, size_t size);

int closeShM(shmADT shm);

#endif