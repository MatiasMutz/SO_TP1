#ifndef _SHMADT_H
#define _SHMADT_H

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct shmCDT *shmADT;

shmADT shmCreate(const char *shmpath);

shmADT shmConnect(char *shmpath);

void shmWrite(shmADT shm, char *buffer, size_t size);

int shmRead(shmADT shm, char *buffer);

void shmClose(shmADT shm);

void shmCloseConnection(shmADT shm);

#endif