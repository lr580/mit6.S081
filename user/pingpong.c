#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    int pp2c[2]; // pipe parent to child
    pipe(pp2c);
    int pc2p[2]; // pipe child to parent
    pipe(pc2p);
    if (fork() != 0) // parent process
    {
        write(pp2c[1], "+", 1); // any char with length 1
        char c;
        read(pc2p[0], &c, 1); // read char of length 1
        printf("%d: received pong\n", getpid());
        wait(0);
    } else {
        char c;
        read(pp2c[0], &c, 1);
        printf("%d: received ping\n", getpid());
        write(pc2p[1], &c, 1);
    }
    exit(0);
}