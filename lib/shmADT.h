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

shmADT createShM();

#endif