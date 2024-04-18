#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

void transmit_data(int l[2], int r[2], int num)
{
    int data;
    while(read(l[RD], &data, sizeof(int)))
    {
        if (data % num) // 如果所读数据为素数，传给右边管道
        {
            write(r[WR], &data, sizeof(int));
        }
    }
    close(l[RD]);
    close(r[WR]);
}

void primes(int lpipe[2])
{
    close(lpipe[WR]);
    int first;
    if (read(lpipe[RD], &first, sizeof(int)) == sizeof(int)) // 判断是否读取到数据
    {
        printf("prime %d\n", first);
        int p[2];
        pipe(p); // 当前管道
        transmit_data(lpipe, p, first);
        if (fork() == 0)
        {
            primes(p); // 递归一个新的进程
        }
        else
        {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}
int main(int argc, char const *argv[])
{
    int pipes[2];
    pipe(pipes);
    for (int i = 2; i <= 35; i++)
    {
        write(pipes[WR], &i, sizeof(int));
    }
    // close(pipes[WR]);
    // primes(pipes);
    if (fork() == 0)
    {
        primes(pipes);
    }
    else
    {
        close(pipes[WR]);
        close(pipes[RD]);
        wait(0);
    }
    exit(0);
}
