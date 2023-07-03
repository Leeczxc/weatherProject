/*
 * 程序名: fileserver.cpp, 文件传输的服务器
 * author: Leeczxc
 */

#include "_public.h"

CLogFile logfile;     // 服务程序的运行日志
CTcpServer TcpServer; // 船舰服务端对象

void FathEXIT(int sig); // 父进程退出函数
void ChldEXIT(int sig); // 子进程退出函数

char strrecvbuffer[1024]; // 发送报文的buffer
char strsendbuffer[1024]; // 接受保文的buffer
// 处理业务的主函数
bool _main(const char *strrecvbuffer, char *strsendbuffer);

// 心跳
bool srv000(const char *strrecvbuffer, char *strsendbuffer);

// 登录业务处理函数
bool srv001(const char *strrecvbuffer, char *strsendbuffer);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Using:./fileserver port logfile timeout.\n Example:./fileserver 5005 ../log/test 35\n\n");
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号，在shell状态下可用"kill + 进程号"正常终止进程
    // 不要用“kill -9 + 进程号”强行终止
    if (logfile.Open(argv[2], "a+") == false)
    {
        printf("logfile.Open(%s) failed.\n", argv[2]);
        return -1;
    }

    // 服务端初始化
    if (TcpServer.InitServer(atoi(argv[1]) == false))
    {
        logfile.Write("TcpServer.InitServer(%s) failed.\n", argv[1]);
        return -1;
    }

    while (true)
    {
        // 等待客户端的连接请求
        if (TcpServer.Accept() == false)
        {
            logfile.Write("TcpServer.Accept() failed.\n");
            FathEXIT(-1);
        }

        logfile.Write("客户端(%s)已连接.\n", TcpServer.GetIP());

        // if(fork() > 0){
        //     TcpServer.CloseClient();
        //     continue;
        // }

        // // 子进程重新设置退出信号
        // signal(SIGINT, ChldEXIT);
        // signal(SIGTERM, ChldEXIT);

        // TcpServer.CloseListen();

        // 子进程与客户端进行通讯，处理业务

        // 与客户端通讯，接受客户端发过来的保文，回复ok
        while (1)
        {
            memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
            memset(strsendbuffer, 0, sizeof(strsendbuffer));

            if (TcpServer.Read(strrecvbuffer, atoi(argv[3])) == false)
            {
                break;
            }
            logfile.Write("接受:%s\n", strrecvbuffer);

            // 处理业务的主函数
            if (_main(strrecvbuffer, strsendbuffer) == false)
            {
                break;
            }

            if (TcpServer.Write(strsendbuffer) == false)
            {
                break;
            }
            logfile.Write("发送:%s\n", strsendbuffer);
        }
        ChldEXIT(-1);
    }
}

// 父进程退出函数
void FathEXIT(int sig)
{
    // 以下代码是为了防止信号处理函数在执行的过程中被信号终端
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    logfile.Write("父进程退出， sig=%d.\n", sig);

    TcpServer.CloseListen(); // 关闭坚挺的socker.

    kill(0, 15); // 通知全部的子进程全部退出
    exit(0);
}