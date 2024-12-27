#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winfinite-recursion"
void solve(int *pipefd) {
    int first_num = 0;
    int forked = 0;
    int passed_num = 0;
    int pipes[2];
    //the first read num must be a prime
    if (read(pipefd[0], &first_num, sizeof first_num) <= 0) {
        close(pipefd[0]);
        exit(0);
    }
    printf("prime %d\n", first_num);
    while (read(pipefd[0], &passed_num, sizeof passed_num) > 0) {
        if (passed_num % first_num) {
            if (!forked) {
                pipe(pipes);
                forked = 1;
                int pid = fork();
                if (pid == 0) {
                    close(pipes[1]);
                    solve(pipes);
                } else {
                    close(pipes[0]);
                }
            }
            write(pipes[1], &passed_num, sizeof passed_num);
        }
    }
    if (forked) {
        close(pipes[1]);
        int child_pid;
        wait(&child_pid);
    }
    close(pipefd[0]);
    exit(0);
}
#pragma GCC diagnostic pop

int main(int argc, char *argv[]) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(2, "create pipe error\n");
        exit(1);
    }
    for (int i = 2; i <= 35; i++) {
        write(pipefd[1], &i, sizeof i);
    }
    close(pipefd[1]);
    solve(pipefd);
    exit(0);
}