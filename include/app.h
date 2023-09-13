#ifndef _APP_H
#define _APP_H

#define FILES_PER_SLAVE 20
#define INITIAL_LOAD 5
#define BLOQUE 5

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void sendToSlave(slave slave[], int fdsAppToSlave[][2], const char *path, int *count, int slaveNumber);
void cleanPath(char *path);
void checkRealloc(const char *s);

#endif