#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void filt(int[2]);
int main(int argc, char **argv)
{
    int input_pipe[2];
    pipe(input_pipe);
    if (fork() != 0) // parent process
    {
        close(input_pipe[0]); // no need to input
        int i;
        for (i = 2; i <= 35; ++i)
        {
            write(input_pipe[1], &i, sizeof i);
        }
        i = -1; // denotes completed
        write(input_pipe[1], &i, sizeof i);
    }
    else
    {
        close(input_pipe[1]); // no need to output
        filt(input_pipe);
        exit(0);
    }
    wait(0);
    exit(0);
}
// pipe left
// read one prime, and delete all its multiples
void filt(int pleft[2])
{
    int p; // prime
    read(pleft[0], &p, sizeof p);
    if (p == -1)
    {
        exit(0);
    }
    printf("prime %d\n", p);

    int pright[2]; // pipe right
    pipe(pright);
    if (fork() != 0) // parent process
    {
        close(pright[0]);
        int val;
        while (read(pleft[0], &val, sizeof val) && val != -1)
        {
            if (val % p)
            {
                write(pright[1], &val, sizeof val);
            }
        }
        val = -1;
        write(pright[1], &val, sizeof val);
        wait(0);
        exit(0);
    }
    else
    {
        close(pright[1]);
        close(pleft[0]);
        filt(pright);
    }
}