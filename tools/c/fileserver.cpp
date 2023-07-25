/*
 * 程序名: fileserver.cpp, 文件传输的服务器
 * author: Leeczxc
 */

#include "_public.h"

// 程序运行的参数结构体
struct st_arg
{
    int clienttype;       // 客户端类型， 1-上传文件，2-下载文件
    char ip[31];          // 服务端ip地址
    int port;             // 服务端的接口
    int ptype;            // 文件上传成功后文件的处理方式: 1-删除文件 2-移动到备份目录
    char clientpath[301]; // 客户端文件存放的根目录
    bool andchild;        // 是否上传clientpath目录下各级子目录的文件， true 是 false 否
    char matchname[301];  //  待上传文件民改得匹配方式， 如 “*.txt, *.xml.”大小写有效
    char srvpath[301];    // 服务端文件存放的根目录
    char srvpathbak[301]; // 服务端文件存放的根目录
    int timetvl;          // 扫描本地目录文件的时间间隔，单位，秒
    int timeout;          // 进程心跳的超时时间
    char pname[51];       // 进程名，建议用"tcpgetifiles_后缀"的方式
} starg;

bool _xmltoarg(char *strxmlbuffer);

CLogFile logfile;     // 服务程序的运行日志
CTcpServer TcpServer; // 船舰服务端对象

void FathEXIT(int sig); // 父进程退出函数
void ChldEXIT(int sig); // 子进程退出函数

char strrecvbuffer[1024]; // 发送报文的buffer
char strsendbuffer[1024]; // 接受保文的buffer
// 处理业务的主函数
// bool _main(const char *strrecvbuffer, char *strsendbuffer);

// 心跳
// bool srv000(const char *strrecvbuffer, char *strsendbuffer);

// 登录业务处理函数
bool ClientLogin();

// 调用文件上传的主函数
void RecvFilesMain();

bool RecvFile(const int sockfd, const char *filename, const char *mtime, int filesize);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Using:./fileserver port logfile\n Example:./fileserver 5005 ../log/tcp\n\n");
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号，在shell状态下可用"kill + 进程号"正常终止进程
    // 不要用“kill -9 + 进程号”强行终止
    CloseIOAndSignal();
    signal(SIGINT, FathEXIT);
    signal(SIGTERM, FathEXIT);

    if (logfile.Open(argv[2], "a+") == false)
    {
        printf("logfile.Open(%s) failed.\n", argv[2]);
        return -1;
    }

    // 服务端初始化
    if (TcpServer.InitServer(atoi(argv[1])) == false)
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

        // 处理登录客户端的登录报文

        // 子进程与客户端进行通讯，处理业务
        if (ClientLogin() == false)
        {
            ChldEXIT(-1);
        }
        // 如果clienttype == 1， 调用上传文件的主函数
        if (starg.clienttype == 1)
        {
            RecvFilesMain();
        }
        // 如果clienttype == 2， 调用下载文件的主函数

        // 与客户端通讯，接受客户端发过来的保文，回复ok
        // while (1)
        // {
        //     memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
        //     memset(strsendbuffer, 0, sizeof(strsendbuffer));

        //     if (TcpServer.Read(strrecvbuffer, atoi(argv[3])) == false)
        //     {
        //         break;
        //     }
        //     logfile.Write("接受:%s\n", strrecvbuffer);

        //     // 处理业务的主函数
        //     if (_main(strrecvbuffer, strsendbuffer) == false)
        //     {
        //         break;
        //     }

        //     if (TcpServer.Write(strsendbuffer) == false)
        //     {
        //         break;
        //     }
        //     logfile.Write("发送:%s\n", strsendbuffer);
        // }

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

    TcpServer.CloseListen(); // 关闭监听的socker.

    kill(0, 15); // 通知全部的子进程全部退出
    exit(0);
}

// 子进程退出服函数
void ChldEXIT(int sig)
{
    // 以下代码是防止信号处理函数在执行的过程中被信号中断
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    logfile.Write("子进程退出, sig=%d.\n", sig);
    TcpServer.CloseClient(); // 关闭客户端的socke
    exit(0);
}

// 处理业务的主函数
// bool _main(const char *strrecvbuffer, char *strsendbuffer)
// {
//     // 解析strrecvbuffer，获取服务代码（业务代码)
//     int isrvcode = -1;
//     GetXMLBuffer(strrecvbuffer, "srvcode", &isrvcode);

//     if (isrvcode != 1)
//     {
//         strcpy(strsendbuffer, "<retcode>-1</retcode><message>用户未登录。</message>");
//         return true;
//     }
//     switch (isrvcode)
//     {
//     case 1: // 登录
//         srv001(strrecvbuffer, strsendbuffer);
//         break;
//     default:
//         logfile.Write("业务代码不合适:%s\n", strrecvbuffer);
//         return false;
//     }
//     return true;
// }

// // 心跳
// bool srv000(const char *strrecvbuffer, char *strsendbuffer)
// {
//     strcpy(strsendbuffer, "<retcode>0</retcode><message>成功。</message>");
//     return true;
// }

// 登录
bool ClientLogin()
{
    // <srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>

    // 解析strrecvbuffer,获取业务参数
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

    if (TcpServer.Read(strrecvbuffer, 20) == false)
    {
        logfile.Write("TcpServer Read() failed.\n");
        return false;
    }

    logfile.Write("strrecvbuffer = %s\n", strrecvbuffer);

    // 解析客户端登录报文
    _xmltoarg(strrecvbuffer);
    if ((starg.clienttype == 1) || (starg.clienttype == 2))
    {
        strcpy(strsendbuffer, "ok");
    }
    else
    {
        strcpy(strsendbuffer, "failed");
    }

    if (TcpServer.Write(strsendbuffer) == false)
    {
        logfile.Write("TcpServer.Write(strsendbuffer) failed\n");
        return false;
    }

    logfile.Write("%s login %s.\n", TcpServer.GetIP(), strrecvbuffer);
    // char tel[21], password[21];
    // GetXMLBuffer(strrecvbuffer, "tel", tel, 20);
    // GetXMLBuffer(strrecvbuffer, "password", password, 20);
    // // 处理业务。 35   // 把处理结果生成strsendbuffer。
    // if ((strcmp(tel, "1392220000") == 0) && (strcmp(password, "123456") == 0))
    // {
    //     strcpy(strsendbuffer, "<retcode>0</retcode><message>成功。</message>");
    // }
    // else
    //     strcpy(strsendbuffer, "<retcode>-1</retcode><message>失败。</message>");

    return true;
}

// 把xml解析到参数starg结构中 47 bool _xmltoarg(char *strxmlbuffer)
bool _xmltoarg(char *strxmlbuffer)
{
    memset(&starg, 0, sizeof(struct st_arg));

    // 不需要对参数做合法性判断，客户端已经判断过了。
    GetXMLBuffer(strxmlbuffer, "clienttype", &starg.clienttype);
    GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
    GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
    GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);
    GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
    GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
    GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);

    GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
    if (starg.timetvl > 30)
        starg.timetvl = 30;

    GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
    if (starg.timeout < 50)
        starg.timeout = 50;

    GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
    strcat(starg.pname, "_srv");

    return true;
}

// 调用文件上传的主函数
void RecvFilesMain()
{
    while (true)
    {
        // 接受客户端的报文
        memset(strsendbuffer, 0, sizeof(strsendbuffer));
        memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

        if (TcpServer.Read(strrecvbuffer, starg.timetvl + 10) == false)
        {
            logfile.Write("TcpServer.Read() failed.\n");
            return;
        }
        logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);

        // 处理心跳报文
        if (strcmp(strrecvbuffer, "<activetest>ok</activetest>") == 0)
        {
            strcpy(strsendbuffer, "ok");
            logfile.Write("strsendbuffer=%s\n", strsendbuffer);
            if (TcpServer.Write(strsendbuffer) == false)
            {
                logfile.Write("TcpServer.Write() failed.\n");
                return;
            }
        }
        // 处理上传文件的请求报文
        if (strncmp(strrecvbuffer, "<filename>", 10) == 0)
        {
            // 解析上传文件请求报文的xml
            char clientfilename[301];
            memset(clientfilename, 0, sizeof(clientfilename));
            char mtime[21];
            memset(mtime, 0, sizeof(mtime));
            int filesize = 0;
            GetXMLBuffer(strrecvbuffer, "filename", clientfilename, 300);
            GetXMLBuffer(strrecvbuffer, "mtime", mtime, 20);
            GetXMLBuffer(strrecvbuffer, "size", &filesize);

            // 客户端和服务端文件的目录是不一样的，以下代码生成服务端的文件名
            // 把文件名的clientpath替换成srvpath，要小心第三个参数
            char serverfilename[301];
            memset(serverfilename, 0, sizeof(serverfilename));
            strcpy(serverfilename, clientfilename);
            UpdateStr(serverfilename, starg.clientpath, starg.srvpath, false);

            // 接受上传文件的内容
            logfile.Write("recv %s(%d) ...", serverfilename, filesize);
            if (RecvFile(TcpServer.m_connfd, serverfilename, mtime, filesize) == true)
            {
                logfile.WriteEx("RecvFile(TcpServer.m_connfd, serverfilename. mtime, filesize) OK. \n");
                SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><result>ok</result>", clientfilename);
            }
            else
            {
                logfile.WriteEx("RecvFile(TcpServer.m_connfd, serverfilename. mtime, filesize) failed. \n");
                SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><result>failed</result>", clientfilename);
            }

            // 把接受结果返回给对端
            logfile.Write("strsendbuffer=%s\n", strsendbuffer);
            if (TcpServer.Write(strsendbuffer) == false)
            {
                logfile.Write("TcpServer.Write() failed.\n");
                return;
            }
        }
    }
}

bool RecvFile(const int sockfd, const char *filename, const char *mtime, int filesize)
{
     
}
