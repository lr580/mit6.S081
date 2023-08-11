#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// the above 3 headers must be includes in such exact orders
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: sleep <ticks>\n");
    }
    int ticks = atoi(argv[1]);
    sleep(ticks);
    exit(0);
}