#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run(char *program, char **args)
{
    if (fork() == 0)
    {
        exec(program, args);
        exit(0);
    }
}
int main(int argc, char *argv[])
{
    // the below purpose is to delete the argv[0]
    //  128 pointers pointing argv
    char *pargs[128];
    // the first position have no argv
    char **args = pargs;
    for (int i = 1; i < argc; ++i)
    {
        *args = argv[i];
        ++args;
    }

    // stored the chars read from stdin
    char buf[2048];
    // p is latest position, last_p is last word's first char's position
    char *p = buf, *last_p = buf;
    // the prefix (cmd to exec xargs)
    char **pa = args;
    // read from stdin
    for (; read(0, p, 1) != 0; ++p)
    {
        if (*p == ' ' || *p == '\n') // get a word
        {
            *p = '\0';
            *pa = last_p; // add a word to pargs
            pa++;
            last_p = p + 1; // next word in future
            if (*p == '\n')
            {
                *pa = 0; // nullptr
                run(argv[1], pargs);
                pa = args; // revoke what read in the line
            }
        }
    }
    // the last line have no \n
    if (pa != args)
    {
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;
        run(argv[1], pargs);
    }
    while (wait(0) != -1)
        ;
    exit(0);
}