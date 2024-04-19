#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAX_BUFSIZE 512

int main(int argc, char *argv[])
{
    char buf[MAX_BUFSIZE + 1] = {0}; // 初始化为0，读到0时，结束输入
    uint occupy = 0;
    char *xargv[MAXARG] = {0};
    int stdin_end = 0;
    // 读取输入的参数
    for (int i = 1; i < argc; i++)
    {
        xargv[i - 1] = argv[i];
    }
    while (!(stdin_end && occupy == 0))
    {
        // 从左往右读取字符
        if (!stdin_end)
        {
            int remain_size = MAX_BUFSIZE - occupy;
            int read_bytes = read(0, buf + occupy, remain_size); // 读取数据到buf+occupy位置上，读取到的字节数为read_bytes
            if (read_bytes < 0)
            {
                fprintf(2, "xargs: read err\n");
            }
            if (read_bytes == 0) // 如果没有数据，则关闭读入
            {
                close(0);
                stdin_end = 1;
            }
            occupy += read_bytes;
        }

        char *result = strchr(buf, '\n'); // buf中的第一个换行符
        while (result)                    // 读到换行符时
        {
            char xbuf[MAX_BUFSIZE + 1] = {0};
            memcpy(xbuf, buf, result - buf); // 将buf[0] - buf[result]的数据存到xbuf中
            xargv[argc - 1] = xbuf;          // 程序执行的语句的存储方式是栈，栈顶的指令先执行
            int ret = fork();
            if (ret == 0)
            {
                // 子进程
                if (!stdin_end)
                {
                    close(0);
                }
                if (exec(argv[1], xargv) < 0)
                {
                    // 程序执行失败
                    fprintf(2, "xargs: exec faild \n");
                    exit(0);
                }
            }
            else
            {
                // 父进程处理缓冲区中剩余的命令
                memmove(buf, result + 1, occupy - 1 - (result - buf));
                occupy -= 1 + (result - buf);                  // 缓冲区长度更新
                memset(buf + occupy, 0, MAX_BUFSIZE - occupy); // 将缓冲区中不需要的部分清零
                int pid;
                wait(&pid); // 等待子进程结束
                result = strchr(buf, '\n'); // 继续下一行
            }
        }
    }
    exit(0);
}
