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
        printf("find: too few args\n");
        exit(0);
    }

    // target = "/"+argv[2] for convenience for cmp
    char target[CHAR_LEN] = {};
    target[0] = '/';
    strcpy(target + 1, argv[2]);
    int hasStar = 0;
    for (char *t = target; *t; ++t)
    {
        if (*t == '*')
        {
            hasStar = 1;
            break;
        }
    }
    if (hasStar == 0)
    {
        target[strlen(target)] = '/';
    }

    find(argv[1], target);
    exit(0);
}
int match(char *re, char *text);
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
    {
        // suffix is target
        // if (strcmp(path + strlen(path) - strlen(target), target) == 0)
        char *subpath = path;
        subpath += strlen(path);
        *subpath = '/';
        --subpath;
        for (; *subpath != '/'; subpath--)
            ;
        // printf("try: %s %s\n", target, subpath);
        if (match(target, subpath))
        {
            printf("%s\n", path);
        }
        break;
    }
    case T_DIR:
    { // make a {} field to contains char buf
        char buf[CHAR_LEN], *p;
        // similar to user/ls.c line 50
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
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

// copy directly from grep.c
int matchhere(char *, char *);
int matchstar(int, char *, char *);

int match(char *re, char *text)
{
    if (re[0] == '^')
        return matchhere(re + 1, text);
    do
    { // must look at empty string
        if (matchhere(re, text))
            return 1;
    } while (*text++ != '\0');
    return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
    if (re[0] == '\0')
        return 1;
    if (re[1] == '*')
        return matchstar(re[0], re + 2, text);
    if (re[0] == '$' && re[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (re[0] == '.' || re[0] == *text))
        return matchhere(re + 1, text + 1);
    return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
    do
    { // a * matches zero or more instances
        if (matchhere(re, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}
