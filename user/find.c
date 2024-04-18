#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char const *directory, char const *filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if ((fd = open(directory, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", directory);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", directory);
        close(fd);
        return;
    }
    strcpy(buf, directory);
    p = buf + strlen(buf);
    *p++ = '/'; //p指向最后一个'/'之后
    while (read(fd, &de, sizeof de) == sizeof de) {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ); //添加路径名称
        p[DIRSIZ] = 0;               //字符串结束标志
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        //不要在“.”和“..”目录中递归
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
            find(buf, filename);
        } else if (strcmp(filename, p) == 0)
            printf("%s\n", buf);
    }

    close(fd);
    // switch (st.type)
    // {
    // case T_FILE:
    //     char *f_name = basename(directory);
    //     break;
    // case T_DIR:
    //     /* code */
    //     break;
    // break;
    // }

}
int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "usage: find [directory] [filename]");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}
