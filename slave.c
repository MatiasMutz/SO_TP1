#include "include/slave.h"

void cleanPath(char *path) {
    for (int i = 0; path[i]; i++) {
        path[i] = '\0';
    }
}

int main(int argc, char *argv[]) {
    char buffer[256] = {'\0'};
    read(STDIN_FILENO, buffer, 256);
    printf("%s\n", buffer);

    pid_t pid;
    int status;
    int wstatus;

    char path[256] = {'\0'};

    for (int i = 0, j = 0; buffer[j]; i++, j++) {
        if (buffer[j] == ' ') {
            if (fork() != 0) {
                /*
                wstatus = waitpid(pid, &status, 0);
                if(wstatus == -1){
                    perror("Waitpid Error");
                    exit(EXIT_FAILURE);
                }
                 */
            }
            else{
                execlp("md5sum", "md5sum", path, NULL);
            }
            cleanPath(path);
            i = -1;
        } else {
            path[i] = buffer[j];
        }

    }
    return 0;

    /*
    char *arguments[] = {"md5sum", "app.c", "slave.c", NULL};

    execlp("md5sum", arguments, (char *) NULL);

    perror("execlp"); // execlp() returns only on error
    exit(EXIT_FAILURE);
     */
}