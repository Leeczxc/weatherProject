#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    // excel使用参数中指定程序替换了当前进程的正文段、数据段、堆和栈
    // 先执行fork函数，创建一个子进程，让紫禁城调用execl执行新的程序
    // 在父进程中，可以调用wait函数等待新程序运行的结束，这样就可以实现调度的功能

    if (argc < 3)
    {
        printf("Using:./procctl timetvl program argv ...\n");
        printf("Example:/tools1/bin/procctl 5 /usr/bin/tar zcvf /tmp/tmp.tgz /usr/include\n\n");

        printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
        printf("timetvl 运行周期，单位：秒。被调度的程序运行结束后，在timetvl秒后会被procctl重新启动。\n");
        printf("program 被调度的程序名，必须使用全路径。\n");
        printf("argvs   被调度的程序的参数。\n");
        printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");

        return -1;
    }

    for (int ii = 1; ii < 64; ii++)
    {
        signal(ii, SIG_IGN);
        close(ii);
    }

    // 生成子进程， 父进程退出，让程序运行在后台，由系统1号进程托管
    if (fork() != 0)
        exit(0);

    // 启用SIGCHLD信号， 让父进程可以wait子进程退出的状态
    signal(SIGCHLD, SIG_DFL);

    // execv函数
    char *pargv[argc];
    for (int ii = 2; ii < argc; ii++)
    {
        pargv[ii - 2] = argv[ii];
    }
    pargv[argc - 2] = NULL;
    while (true)
    {
        if (fork() == 0)
        {
            execv(argv[2], pargv);
            exit(0);
        }
        else
        {
            int status;
            wait(&status);
            sleep(atoi(argv[1]));
        }
    }
    // while(true){
    //     if(fork() == 0){
    //         execl(argv[2], argv[2], argv[3], argv[4], (char*)0);
    //         if(argc == 3)
    //             execl(argv[2], argv[2],(char*)0);
    //     }else{
    //         int status;
    //         wait(&status);
    //         sleep(atoi(argv[1]));
    //     }
    // }
    return 0;
}