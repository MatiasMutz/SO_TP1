#ifndef _SHMADT_H
#define _SHMADT_H

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

typedef struct shmCDT *shmADT;

shmADT create_shm(const char* shmpath);

shmADT connect_shm(char *shmpath);

void write_shm(shmADT shm, char *buffer, size_t size);

void read_shm(shmADT shm, char *buffer);

void close_shm(shmADT shm);

#endif