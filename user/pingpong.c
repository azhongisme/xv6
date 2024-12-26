#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pipefd[2];
    char buf[128];
    if (pipe(pipefd) == -1) {
        fprintf(2, "create pipe error\n");
        exit(1);
    }
    int fd = fork();
    if (fd == -1) {
        fprintf(2, "fork error\n");
        exit(1);
    } else if (fd == 0) {
        int pid = getpid();
        read(pipefd[0], buf, sizeof(buf));
        close(pipefd[0]);
        fprintf(1, "%d: received %s\n", pid, buf);
        write(pipefd[1], "ping", 4);
        close(pipefd[1]);
        exit(0);
    } else {
        int pid = getpid();
        write(pipefd[1], "pong", 4);
        close(pipefd[1]);
        read(pipefd[0], buf, sizeof(buf));
        close(pipefd[0]);
        fprintf(1, "%d: received %s\n", pid, buf);
        exit(0);
    }
    exit(0);
}