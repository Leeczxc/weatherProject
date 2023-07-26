/*
 * 程序名: 采用tcp协议，实现文件上传的客户端
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
    char srvpath[301];    // 本地文件存放的根目录
    char srvpathbak[301]; // 文件成功上传后，服务器文件备份的根目录，当ptype == 2时有效
    bool andchild;        // 是否上传serverpath目录下各级子目录的文件， true 是 false 否
    char matchname[301];  //  待上传文件民改得匹配方式， 如 “*.txt, *.xml.”大小写有效
    char clientpath[301]; // 客户端文件存放的根目录
    int timetvl;          // 扫描服务器目录文件的时间间隔，单位，秒
    int timeout;          // 进程心跳的超时时间
    char pname[51];       // 进程名，建议用"tcpgetifiles_后缀"的方式
} starg;

// 程序退出和信号2、15的处理函数
void EXIT(int sig);

void _help();

// 登录业务
bool Login(const char *argv);
// 把xm解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);
// 日志文件
CLogFile logfile;

CPActive PActive; // 进程心跳

CTcpClient TcpClient;

char strrecvbuffer[1024]; // 发送报文的buffer
char strsendbuffer[1024]; // 接受保文的buffer

//  bool ActiveTest(); // 心跳

// bool SendFile(const int sockfd, const char *filename, const int filesize);

void _tcpgetfiles(); // 调用文件上传的主函数，执行一次文件上传的任务

bool RecvFile(const int sockfd, const char *filename, const char *mtime, int filesize); // 接受文件
// bool bcontinue = true; // 如果调用_tcpgetfiles()发送了文件，bcontinue为true， 初始化为true

// bool AckMessage(const char *strrecvbuffer); // 文件上传后文件的处理方式

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        _help();
        return -1;
    }

    // 关闭全部的信号和输入输出。
    // 设置信号，在shell状态下可用"kill + 进程号"正常终止进程
    // 不要用“kill -9 + 进程号”强行终止
    // CloseIOAndSignal();
    // signal(SIGINT, EXIT);
    // signal(SIGTERM, EXIT);

    if (logfile.Open(argv[1], "a+") == false)
    {
        printf("open logfile error!\n");
        return -1;
    }

    // 解析xml，得到程序运行的参数
    if (_xmltoarg(argv[2]) == false)
    {
        return -1;
    }
    PActive.AddPInfo(starg.timeout, starg.pname); // 把进程的心跳信息写入共享内存
    // 向服务端发送连接请求
    if (TcpClient.ConnectToServer(starg.ip, starg.port) == false)
    {
        logfile.Write("TcpClient.ConnectToServer(%s, %s) failed.\n", starg.ip, starg.port);
        EXIT(-1);
    }

    // 登录业务
    if (Login(argv[2]) == false)
    {
        logfile.Write("Login() failed.\n");
        EXIT(-1);
    }

    // 调用文件下载的主函数
    _tcpgetfiles();

    // if (bcontinue == false)
    // {
    //     sleep(starg.timetvl);
    //     if (ActiveTest() == false)
    //     {
    //         break;
    //     }
    // }

    PActive.UptATime();

    // for (int ii = 3; ii < 5; ii++)
    // {
    //     if (ActiveTest() == false)
    //         break;
    //     sleep(ii);
    // }
    // while(true){
    //     PActive.UptATime();
    // }
    // EXIT(0);
    EXIT(0);
}

// 心跳
// bool ActiveTest()
// {
//     memset(strsendbuffer, 0, sizeof(strsendbuffer));
//     memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
//     SPRINTF(strsendbuffer, sizeof(strsendbuffer), "<activetest>ok</activetest>");
//     // logfile.Write("发送: %s\n", strsendbuffer);
//     if (TcpClient.Write(strsendbuffer) == false)
//     { // 向服务端发送请求报文
//         logfile.Write("心跳 tcpClient.Write(buffer) failed\n");
//         return false;
//     }

//     if (TcpClient.Read(strrecvbuffer, 20) == false)
//     { // 接受服务端的回应报文
//         logfile.Write("心跳 tcpClient.Read(strrecvbuffer, 20) failed\n");
//         return false;
//     }
//     // logfile.Write("接受: %s\n", strrecvbuffer);
//     return true;
// }

bool Login(const char *argv)
{

    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

    SPRINTF(strsendbuffer, sizeof(strsendbuffer), "%s<clienttype>2</clienttype>", argv);

    logfile.Write("发送: %s\n", strsendbuffer);
    if (TcpClient.Write(strsendbuffer) == false)
    {
        logfile.Write("TcpClient.Write(strsendbuffer) failed\n");
        return false;
    }

    if (TcpClient.Read(strrecvbuffer, 20) == false)
    { // 接受服务端的回应报文
        logfile.Write("TcpClient.Read(strrecvbuffer, 20) failed\n");
        return false;
    }
    logfile.Write("接受: %s\n", strrecvbuffer);

    logfile.Write("登录(%s:%d)成功.\n", starg.ip, starg.port);
    return true;
}

void EXIT(int sig)
{
    logfile.Write("程序退出, sig = %d\n\n", sig);
    exit(0);
}

void _help()
{
    printf("\n");
    printf("Using:/project/tools1/bin/tcpgetfiles logfilename xmlbuffer\n\n");

    printf("Sample:/project/tools1/bin/procctl 20 /project/tools1/bin/tcpgetfiles /log/idc/tcpgetfiles_surfdata.log \"<ip>192.168.174.132</ip><port>5005</port><ptype>1</ptype><srvpath>/tmp/tcp/surfdata2</srvpath><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcp/surfdata3</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>\"\n");
    printf("       /project/tools1/bin/procctl 20 /project/tools1/bin/tcpgetfiles /log/idc/tcpgetfiles_surfdata.log \"<ip>192.168.174.132</ip><port>5005</port><ptype>2</ptype><srvpath>/tmp/tcp/surfdata2</srvpath><srvpathbak>/tmp/tcp/surfdata2bak</srvpathbak><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcp/surfdata3</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>\"\n\n\n");

    printf("本程序是数据中心的公共功能模块，采用tcp协议从服务端下载文件。\n");
    printf("logfilename   本程序运行的日志文件。\n");
    printf("xmlbuffer     本程序运行的参数，如下：\n");
    printf("ip            服务端的IP地址。\n");
    printf("port          服务端的端口。\n");
    printf("ptype         文件下载成功后服务端文件的处理方式：1-删除文件；2-移动到备份目录。\n");
    printf("srvpath       服务端文件存放的根目录。\n");
    printf("srvpathbak    文件成功下载后，服务端文件备份的根目录，当ptype==2时有效。\n");
    printf("andchild      是否下载srvpath目录下各级子目录的文件，true-是；false-否，缺省为false。\n");
    printf("matchname     待下载文件名的匹配规则，如\"*.TXT,*.XML\"\n");
    printf("clientpath    客户端文件存放的根目录。\n");
    printf("timetvl       扫描服务目录文件的时间间隔，单位：秒，取值在1-30之间。\n");
    printf("timeout       本程序的超时时间，单位：秒，视文件大小和网络带宽而定，建议设置50以上。\n");
    printf("pname         进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n");
}

// 把xml解析到参数starg结构
bool _xmltoarg(char *strxmlbuffer)
{
    memset(&starg, 0, sizeof(struct st_arg));

    GetXMLBuffer(strxmlbuffer, "ip", starg.ip);
    if (strlen(starg.ip) == 0)
    {
        logfile.Write("ip is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "port", &starg.port);
    if (starg.port == 0)
    {
        logfile.Write("port is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
    if ((starg.ptype != 1) && (starg.ptype != 2))
    {
        logfile.Write("ptype not in (1,2).\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
    if (strlen(starg.srvpath) == 0)
    {
        logfile.Write("srvpath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);
    if ((starg.ptype == 2) && (strlen(starg.srvpathbak) == 0))
    {
        logfile.Write("srvpathbak is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);

    GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
    if (strlen(starg.matchname) == 0)
    {
        logfile.Write("matchname is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
    if (strlen(starg.clientpath) == 0)
    {
        logfile.Write("clientpath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
    if (starg.timetvl == 0)
    {
        logfile.Write("timetvl is null.\n");
        return false;
    }

    // 扫描服务端目录文件的时间间隔，单位：秒。
    // starg.timetvl没有必要超过30秒。
    if (starg.timetvl > 30)
        starg.timetvl = 30;

    // 进程心跳的超时时间，一定要大于starg.timetvl，没有必要小于50秒。
    GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
    if (starg.timeout == 0)
    {
        logfile.Write("timeout is null.\n");
        return false;
    }
    if (starg.timeout < 50)
        starg.timeout = 50;

    GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
    if (strlen(starg.pname) == 0)
    {
        logfile.Write("pname is null.\n");
        return false;
    }

    return true;
}

void _tcpgetfiles(){
    PActive.UptATime();
    while (true)
    {
        // 接受客户端的报文
        memset(strsendbuffer, 0, sizeof(strsendbuffer));
        memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

        PActive.UptATime();

        // 接受服务端的报文
        // 第二个参数的取值必须大于starg.timetvl, 小于starg.timeout
        if (TcpClient.Read(strrecvbuffer, starg.timetvl + 10) == false)
        {
            logfile.Write("TcpClinet.Read() timeout.\n");
            return;
        }
        // logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);

        // 处理心跳报文
        if (strcmp(strrecvbuffer, "<activetest>ok</activetest>") == 0)
        {
            strcpy(strsendbuffer, "ok");
            // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
            if (TcpClient.Write(strsendbuffer) == false)
            {
                logfile.Write("TcpClient.Write() failed.\n");
                return;
            }
        }
        // 处理下载文件的请求报文

        if (strncmp(strrecvbuffer, "<filename>", 10) == 0)
        {
            // 解析下载文件请求报文的xml
            char serverfilename[301];
            memset(serverfilename, 0, sizeof(serverfilename));
            char mtime[21];
            memset(mtime, 0, sizeof(mtime));
            int filesize = 0;
            GetXMLBuffer(strrecvbuffer, "filename", serverfilename, 300);
            GetXMLBuffer(strrecvbuffer, "mtime", mtime, 20);
            GetXMLBuffer(strrecvbuffer, "size", &filesize);

            // 客户端和服务端文件的目录是不一样的，以下代码生成客户端的文件名
            // 把文件名的srvp替换成clientpath，要小心第三个参数
            char clientfilename[301];
            memset(clientfilename, 0, sizeof(clientfilename));
            strcpy(clientfilename, serverfilename);
            UpdateStr(clientfilename, starg.srvpath, starg.clientpath, false);

            // 接受下载文件的内容
            logfile.Write("recv %s(%d) ...", clientfilename, filesize);
            if (RecvFile(TcpClient.m_connfd, clientfilename, mtime, filesize) == true)
            {
                logfile.WriteEx("OK. \n");
                SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><result>ok</result>", serverfilename);
            }
            else
            {
                logfile.WriteEx("failed. \n");
                SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><result>failed</result>", serverfilename);
            }

            // 把接受结果返回给对端
            // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
            if (TcpClient.Write(strsendbuffer) == false)
            {
                logfile.Write("TcpServer.Write() failed.\n");
                return;
            }
        }
    }
}

// 接受文件的内容
bool RecvFile(const int sockfd, const char *filename, const char *mtime, int filesize)
{
    // 生成临时文件名
    char strfilenametmp[301];
    SNPRINTF(strfilenametmp, sizeof(strfilenametmp), 301, "%s.tmp", filename);

    int totalbytes = 0; // 已接受文件的总字节数
    int onread = 0; // 本次打算接受的字节数
    char buffer[1000]; // 接受文件内容的缓冲区

    FILE* fp = NULL;
    // 创建临时文件
    if((fp = FOPEN(strfilenametmp, "wb")) == NULL){
        logfile.Write("OPEN(strfilenametmp, \"wb\") failed.\n");
        return false;
    }

    while(true){
        memset(buffer, 0, sizeof(buffer));
        // 计算本次应该接受的字节数
        if(filesize - totalbytes > 1000){
            onread = 1000;
        }else{
            onread = filesize - totalbytes;
        }
        // 接收文件内容
        if(Readn(sockfd, buffer, onread) == false){
            logfile.Write("readn failed. \n");
            fclose(fp);
            return false;
        }
        // 把接受的内容写入文件
        fwrite(buffer, 1, onread, fp);

        // 计算已接受文件的总字节数，如果文件接受完， 跳出循环
        totalbytes = totalbytes + onread;
        if(totalbytes == filesize){
            // logfile.Write("%s.tmp have recvfile done.\n", filename);
            break;
        }
    }

    // 关闭临时文件
    fclose(fp);
    // 重置文件的时间
    UTime(strfilenametmp, mtime);

    // 把临时文件RENAME为正式的时间
    if(RENAME(strfilenametmp, filename) == false){
        logfile.Write("file changed name failed.\n");
        return false;
    }
    return true;
}

// 调用文件下载的主函数
// bool _tcpgetfiles()
// {
//     CDir Dir;
//     // 调用OpenDir()大佬starg.clientpath目录
//     if (Dir.OpenDir(starg.clientpath, starg.matchname, 10000, starg.andchild) == false)
//     {
//         logfile.Write("Dir.openDir(%s) 失败\n", starg.clientpath);
//     }

//     int delayed = 0; // 未收到对端确认报文的文件数量
//     int buflen = 0;  // 用于存放strrecvbuffer的长度
//     bcontinue = false;
//     while (true)
//     {
//         // 初始化
//         memset(strsendbuffer, 0, sizeof(strsendbuffer));
//         memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
//         // 遍历目录中的每个文件，调用ReadDir()获取一个文件名
//         if (Dir.ReadDir() == false)
//         {
//             break;
//         }
//         bcontinue = true;

//         // 把文件名、修改时间、文件大小组成报文，发送给对端
//         SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000, "<filename>%s</filename><mtime>%s</mtime><size>%d</size>",
//                  Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);
//         // logfile.Write("strsendbuffer=%s\n", strsendbuffer);
//         if (TcpClient.Write(strsendbuffer) == false)
//         {
//             logfile.Write("TcpClient.Write() failed.\n");
//             return false;
//         }
//         // 把文件的内容发送给对端
//         logfile.Write("send %s(%d) ... ", Dir.m_FullFileName, Dir.m_FileSize);
//         if (SendFile(TcpClient.m_connfd, Dir.m_FullFileName, Dir.m_FileSize) == true)
//         {
//             logfile.WriteEx("ok. \n");
//             delayed++;
//         }
//         else
//         {
//             logfile.Write("failed. \n");
//             TcpClient.Close();
//             return false;
//         }
//         PActive.UptATime();

//         // 接收对端的确认报文
//         // if (TcpClient.Read(strrecvbuffer, 20) == false)
//         // {
//         //     logfile.Write("TcpClient.Read() failed.\n");
//         //     return false;
//         // }

//         while (delayed > 0)
//         {
//             memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
//             if (TcpRead(TcpClient.m_connfd, strrecvbuffer, &buflen, -1) == false)
//             {
//                 break;
//             }
//             delayed--;
//             // logfile.Write("strrecvbuffer=%s\n", strrecvbuffer);
//             // 删除或者转存本地的文件
//             AckMessage(strrecvbuffer);
//         }
//     }

//     while (delayed > 0)
//     {
//         memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
//         if (TcpRead(TcpClient.m_connfd, strrecvbuffer, &buflen, 10) == false)
//         {
//             break;
//         }
//         delayed--;
//         AckMessage(strrecvbuffer);
//     }

//     return true;
// }

// bool SendFile(const int sockfd, const char *filename, const int filesize)
// {
//     int onread = 0;     // 每次调用fread时打算读取的字节数
//     int bytes = 0;      // 调用一次fread从文件中读取的文件数
//     char buffer[1000];  // 从文件读取数据的buffer
//     int totalbytes = 0; // 从文件中已读取的字节总数
//     FILE *fp = NULL;
//     // 以"rb"的模式打开文件
//     if ((fp = fopen(filename, "rb")) == NULL)
//     {
//         logfile.Write("Open(%s) failed.\n", filename);
//         return false;
//     }
//     while (true)
//     {
//         memset(buffer, 0, sizeof(buffer));
//         // 计算本次应该读取的字节数， 如果剩余的数据超过1000字节，就打算读1000字节
//         if (filesize - totalbytes > 1000)
//         {
//             onread = 1000;
//         }
//         else
//         {
//             onread = filesize - totalbytes;
//         }
//         // 从文件中读取数据
//         bytes = fread(buffer, 1, onread, fp);
//         // 把读取的数据发送给对端
//         if (bytes > 0)
//         {
//             if (Writen(sockfd, buffer, bytes) == false)
//             {
//                 logfile.Write("Writen(sockfd, buffer, bytes) failed.\n");
//                 fclose(fp);
//                 return false;
//             }
//         }
//         // 计算文件未读取的字节数，如果文件已读完，跳出循环
//         totalbytes = totalbytes + bytes;

//         if (totalbytes == filesize)
//         {
//             // logfile.Write("%s send done.\n", filename);
//             break;
//         }
//     }
//     fclose(fp);
//     return true;
// }

// bool AckMessage(const char *strrecvbuffer)
// {
//     char filename[301];
//     char result[11];

//     memset(filename, 0, sizeof(filename));
//     memset(result, 0, sizeof(result));

//     GetXMLBuffer(strrecvbuffer, "filename", filename, 300);
//     GetXMLBuffer(strrecvbuffer, "result", result, 10);

//     // 如果服务端接收文件失败
//     if (strcmp(result, "ok") != 0)
//     {
//         return false;
//     }
//     // ptype == 1, 删除文件
//     if (starg.ptype == 1)
//     {
//         if (REMOVE(filename) == false)
//         {
//             logfile.Write("rm %s false.\n", filename);
//             return false;
//         }
//     }

//     // ptype == 2, 移动到备份目录
//     if (starg.ptype == 2)
//     {
//         // 生成备份目录的文件名
//         char bakfilename[301];
//         STRCPY(bakfilename, sizeof(bakfilename), filename);
//         UpdateStr(bakfilename, starg.clientpath, starg.clientpathbak, false);
//         if (RENAME(filename, bakfilename) == false)
//         {
//             logfile.Write("REMOVE(%s, %s) failed.\n", filename, bakfilename);
//             return false;
//         }
//     }
//     return true;
// }
