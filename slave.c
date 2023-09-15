#include "lib/slave.h"

#define PATH_SIZE 128
#define BUFFER_SIZE 256

static void cleanPath(char *path);

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    while (1) {
        char buffer[BUFFER_SIZE] = {'\0'};
        if (read(STDIN_FILENO, buffer, BUFFER_SIZE) == 0) {
            return 0;
        }

        pid_t pid;
        int status;
        int wstatus;

        char path[PATH_SIZE] = {'\0'};

        for (int i = 0, j = 0; buffer[j]; i++, j++) {
            if (buffer[j] == ' ') {
                if ((pid = fork()) != 0) {
                    wstatus = waitpid(pid, &status, 0);
                    if (wstatus == -1) {
                        perror("Waitpid Error");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    execlp("md5sum", "md5sum", path, (char *)NULL);
                    perror("Error in execve.");
                    exit(1);
                }
                cleanPath(path);
                i = -1;
            } else {
                path[i] = buffer[j];
            }
        }
    }
    return 0;
}

void cleanPath(char *path) {
    for (int i = 0; path[i]; i++) {
        path[i] = '\0';
    }
}