#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    char buf = 'P'; //transact buffer

    int fd_c2p[2]; // child to parent
    int fd_p2c[2]; // parent to child
    pipe(fd_c2p);
    pipe(fd_p2c);

    int pid = fork();
    int exit_status = 0;

    if (pid == 0) { //child process
        close(fd_p2c[1]);
        close(fd_c2p[0]);

        if (read(fd_p2c[0], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child read() error!\n");
            exit_status = 1; //an exception occurs
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if (write(fd_c2p[1], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write() error!\n");
            exit_status = 1;
        }

        close(fd_p2c[0]);
        close(fd_c2p[1]);

        exit(exit_status);
    } else { // parent process
        close(fd_p2c[0]);
        close(fd_c2p[1]);

        if (write(fd_p2c[1], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() error!\n");
            exit_status = 1;
        }

        if (read(fd_c2p[0], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent read() error!\n");
            exit_status = 1; // an exception occurs
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(fd_p2c[1]);
        close(fd_c2p[0]);

        exit(exit_status);
    }
}
