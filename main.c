#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <sys/syscall.h>
#include <capstone/capstone.h>


#define BUFFER_SIZE 1024

int main() {

    int pipefd[2];
    char buffer[BUFFER_SIZE];


    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t target;
    if ((target = fork()) == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        ptrace(PTRACE_TRACEME);
        char *const argv[] = {"./build/bin", "meister", NULL};
        execv(argv[0], argv);
        perror("execv");
        exit(1);
    } else {
        struct user_regs_struct regs;

        close(pipefd[1]);

        int status;
        int s = waitpid(target, &status, 0); if (s == -1) {
            fprintf(stderr, "Could not stop the process!\n");
            exit(EXIT_FAILURE);
        }

        while (!WIFEXITED(status)) {
          if (ptrace(PTRACE_GETREGS, target, NULL, &regs) == -1) {
            fprintf(stderr, "Could not read Registers!\n");
            exit(EXIT_FAILURE);
          }

          printf("rax: %lld\n", regs.rax);
          if (regs.rax == SYS_write) {
            printf("[0x%lx] -> hit syscall write (%lld)\n", regs.rip, regs.rax);
            getchar();
          }

          if (ptrace(PTRACE_SINGLESTEP, target, NULL, NULL) == -1) {
            fprintf(stderr, "Could not singlestep!\n");
            exit(EXIT_FAILURE);
          };

          int s = waitpid(target, &status, 0);
          if (s == -1) {
            fprintf(stderr, "Could not stop the process!\n");
            exit(EXIT_FAILURE);
          }
        }


        size_t n = read(pipefd[0], buffer, sizeof(buffer) / sizeof(buffer[0]));
        if ( n <= 0) {
            fprintf(stderr, "couldnt read any data from child process\n");
            exit(1);
        }
        if (n >= sizeof(buffer) / sizeof(buffer[0])) {
            fprintf(stderr, "buffer too big");
            exit(1);
        }

        buffer[n] = '\0';
        printf("data: %s\n", buffer);
        close(pipefd[0]);

        s = waitpid(target, &status, 0); if (s == -1) {
            fprintf(stderr, "Process didnt end properly or we didnt notice!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}