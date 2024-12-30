#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

const int BUFSIZE = 512;

int main(int argc, char *argv[]) {
  char buf[BUFSIZE];
  uint occupy = 0;
  char *xarg[MAXARG] = {"\0"};
  int cnt = 0;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-n") == 0) {
      i = 2;
      continue;
    }
    xarg[cnt++] = argv[i];
  }

  while (1) {
    int read_bytes = read(0, buf + occupy, 1);
    if (read_bytes < 0) {
      fprintf(2, "xargs: read error\n");
      exit(1);
    }
    if (read_bytes == 0) {
      break;
    }

    occupy += read_bytes;

    if (*(buf + occupy - 1) == '\n') {
      int pid = fork();
      buf[occupy-1]='\0';
      xarg[cnt] = buf;
      if (pid == 0) {
        exec(xarg[0], xarg);
      } else {
        int child_pid = 0;
        wait(&child_pid);
      }
      occupy = 0;
    }

  }
  exit(0);
}
