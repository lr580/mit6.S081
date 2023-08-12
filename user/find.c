#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define CHAR_LEN 512
void find(char *, char *);
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("find: too few args");
        exit(0);
    }

    // target = "/"+argv[2] for convenience for cmp
    char target[CHAR_LEN];
    target[0] = '/';
    strcpy(target + 1, argv[2]);

    find(argv[1], target);
    exit(0);
}
// curr path: path, relative name to find: target
void find(char *path, char *target)
{
    int fd;
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        // suffix is target
        if (strcmp(path + strlen(path) - strlen(target), target) == 0)
        {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
    {//make a {} field to contains char buf
        char buf[CHAR_LEN], *p;
        // similar to user/ls.c line 50
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: path too long\n");
            break;
        }

        strcpy(buf, path);     // path prefix
        p = buf + strlen(buf); // pointer move
        *p++ = '/';            // cur pointer

        struct dirent de;
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            // similar to user/ls.c
            if (de.inum == 0)
            {
                continue;
            }
            // copy filename and add into buf, do not change position of p
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = '\0';

            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            // skip . and ..
            if (strcmp(buf + strlen(buf) - 2, "/.") != 0 &&
                strcmp(buf + strlen(buf) - 3, "/..") != 0)
            {
                find(buf, target);
            }
        }
        break;
    }
    }
    close(fd);
}