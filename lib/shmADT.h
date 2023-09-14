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

shmADT create_shm(const char* shmpath);

int connect_shm(shmADT shm, char *shmpath);

int write_shm(shmADT shm, char *buffer, size_t size);

int read_shm(shmADT shm, char *buffer, size_t size);

int close_shm(shmADT shm);

#endif