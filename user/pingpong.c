#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    char buf[]  = {'B'}; //用于传输的字节

    int fd_c2p[2]; //子 -> 父
    int fd_p2c[2]; //父 -> 子
    pipe(fd_c2p);
    pipe(fd_p2c);

    int ret = fork();
    int pid; //获取进程id
    if(ret == 0)
    {
        //子进程
        pid = getpid();
        //互斥锁
        close(fd_p2c[1]);
        close(fd_c2p[0]);

        read(fd_p2c[0],buf,1);
        printf("%d: received ping\n",pid);
        write(fd_c2p[1],buf,1);
        
        close(fd_p2c[0]);
        close(fd_c2p[1]);
        exit(0);
    }else{
        //父进程
        pid = getpid();
        close(fd_c2p[1]);
        close(fd_p2c[0]);

        write(fd_p2c[1],buf,1);
        read(fd_c2p[0],buf,1);
        printf("%d: received pong\n",pid);

        close(fd_p2c[1]);
        close(fd_c2p[0]);
        exit(0);
    }
    
}

