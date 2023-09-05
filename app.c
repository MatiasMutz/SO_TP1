#include "include/app.h"

#define FILES_PER_SLAVE 20
#define INITIAL_LOAD 5

int main(int argc, char* argv[]) {
    int filesQty = argc - 1;

    int slavesQty = (int)ceil(filesQty / FILES_PER_SLAVE);

    int status;
    pid_t pid;
    pid_t wpid;

    int count = 1;

    for (int i = 0; i < slavesQty; i++) {
        if ((pid = fork()) != 0) {
        } else {
            char* aux[INITIAL_LOAD + 3];
            aux[0] = "slave";
            for (int j = 1; j <= INITIAL_LOAD + 1 && count < filesQty; j++, count++) {
                aux[j] = argv[count];
            }
            aux[INITIAL_LOAD + 2] = NULL;
            execve("slave", aux, NULL);
        }
    }
    while ((wpid = waitpid(-1, &status, 0)) > 0)
        ;
}