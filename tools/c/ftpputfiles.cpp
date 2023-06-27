#include "_public.h"
#include "_ftp.h"
// 文件的参数
// /home/Leeczxc/Project/weatherProject/idcdata/test.log "<host>localhost:21</host><mode>1</mode><username>Leeczxc</username><password>alichao1</password><localpath>/home/Leeczxc/Project/weatherProject/test/client</localpath><remotepath>/home/Leeczxc/Project/weatherProject/test/server</remotepath><matchname>*</matchname><listfilename>/home/Leeczxc/Project/weatherProject/idcdata/fileinfo</listfilename><ptype>1</ptype><localpathbak>/home/Leeczxc/Project/weatherProject/idcdata</localpathbak><okfilename>/home/Leeczxc/Project/weatherProject/idcdata/okfilename</okfilename><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"
struct st_arg
{
    char host[31];           // 远程服务的ip和端口
    int mode;                // 传输模式,1-被动模式，2-主动模式，缺省采用被动模式
    char username[31];       // 远程服务器ftp的用户名
    char password[31];       // 远程服务器ftp的密码
    char remotepath[301];    // 远程服务器存放文件的目录
    char localpath[301];     // 本地文件存放的目录
    char matchname[101];     // 待上传文件匹配的规则
    int ptype;               // 文件上传成功后，客户端文件的处理方式：1-什么也不做，2-删除，3-备份
    char localpathbak[301];   // 文件上传后客户端文件的备份的目录
    char okfilename[301];    // 已上传成功文件名清单
    int timeout;             // 进程心跳的超时时间
    char pname[51];          // 进程名，建议用"ftpgetfiles_"后缀的方式
} starg;

struct st_fileinfo
{
    char filename[301]; // 文件名
    char mtime[21];     // 文件时间
};

vector<struct st_fileinfo> vfileinfo1; // 存放已经上传成功的文件名的容器，okfilename中加载
vector<struct st_fileinfo> vfileinfo2; // 存放上传前列出客户端文件名的窗口
vector<struct st_fileinfo> vfileinfo3; // 本次不需要上传的文件的容器
vector<struct st_fileinfo> vfileinfo4; // 本次需要上传的文件的容器

// 加载okfilename文件中的内容到容器vlistfile1中
bool LoadOKFile();

// 比较vlistfile2和vlistfile1， 得到vlistfile3和 vlistfile4

bool CompVector();
// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的就okfilename文件

bool WriteToOkFile();

// 把vlistfile4中的内容到vlistfile2中
bool AppenedToOKFile(struct st_fileinfo *stfileinfo);

CLogFile logfile;

Cftp ftp;

// 程序退出函数和信号2\15的处理函数

void EXIT(int sig);

void _help();

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

// 上传文件
bool _ftpputfiles();

// 把localpath目录下的文件加载到vlistfile2容器中
bool LoadLocalFile();

// 心跳对象
CPActive PActive; // 进程心跳

int main(int argc, char *argv[])
{
    // 第一步计划，把服务器上某目录的文件全部上传到本地目录（可以指定文件名的匹配规则\）
    // 日志文件名 ftp服务器的ip和端口 传输模式 ftp用户名 ftp的密码 服务器存放的文件的目录 本地存放文件的目录 上传文件名匹配的规则
    // 参数存放在xml中
    if (argc != 3)
    {
        _help();
        return -1;
    }
    // 处理程序的退出信号
    CloseIOAndSignal();
    signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);

    // 打开日志文件
    if (logfile.Open(argv[1], "a+") == false)
    {
        printf("打开日志文件失败(%s).\n", argv[1]);
        return -1;
    }
    // 解析xml，得到程序运行的参数
    if (_xmltoarg(argv[2]) == false)
    {
        return -1;
    }

    PActive.AddPInfo(starg.timeout, starg.pname);
    // 登录ftp服务器
    if (ftp.login(starg.host, starg.username, starg.password, starg.mode) == false)
    {
        logfile.Write("starg.host %sLogin failed", starg.host);
    }

    logfile.Write("login success!\n");
    // 上传
    _ftpputfiles();

    // 登出
    ftp.logout();

    return 0;
}

void EXIT(int sig)
{
    printf("程序退出，sig=%d\n\n", sig);
    exit(0);
}

void _help()
{
    printf("本程序是通用的功能模块，用于把远程ftp服务端的文件上传到本地目录。\n");
    printf("logfilename是本程序运行的日志文件。\n");
    printf("xmlbuffer为文件上传的参数，如下：\n");
    printf("<host>127.0.0.1:21</host> 远程服务端的IP和端口。\n");
    printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
    printf("<username>wucz</username> 远程服务端ftp的用户名。\n");
    printf("<password>wuczpwd</password> 远程服务端ftp的密码。\n");
    printf("<remotepath>/tmp/idc/surfdata</remotepath> 远程服务端存放文件的目录。\n");
    printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
    printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待上传文件匹配的规则。");
    printf("不匹配的文件不会被上传，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
    printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename> 上传前列出服务端文件名的文件。\n");
    printf("<ptype>1</ptype> 文件上传成功后，远程服务端文件的处理方式：1-什么也不做；2-删除；3-备份，如果为3，还要指定备份的目录。\n");
    printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件上传成功后，服务端文件的备份目录，此参数只有当ptype=3时才有效。\n");
    printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> 已上传成功文件名清单，此参数只有当ptype=1时才有效。\n");
    printf("<checkmtime>true</checkmtime> 是否需要检查服务端文件的时间，true-需要，false-不需要，此参数只有当ptype=1时才有效，缺省为false。\n");
    printf("<timeout>80</timeout> 上传文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
    printf("<pname>ftpgetfiles_surfdata</pname> 进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n");
}

bool _xmltoarg(char *strxmlbuffer)
{
    memset((void *)&starg, 0, sizeof(struct st_arg));
    GetXMLBuffer(strxmlbuffer, "host", starg.host, 30); // 远程服务器的ip和端口
    if (strlen(starg.host) == 0)
    {
        logfile.Write("host is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "mode", &starg.mode); // 传输模式， 看上面的注释
    if (starg.mode != 2)
    {
        starg.mode = 1;
    }

    GetXMLBuffer(strxmlbuffer, "username", starg.username, 30); // 远程服务器ftp的用户名
    if (strlen(starg.username) == 0)
    {
        logfile.Write("username is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "password", starg.password, 30); // 远程服务器的密码
    if (strlen(starg.password) == 0)
    {
        logfile.Write("password is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "remotepath", starg.remotepath, 300); // 远程服务器存放文件的目录
    if (strlen(starg.remotepath) == 0)
    {
        logfile.Write("Remotapath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "localpath", starg.localpath, 300); // 本地存放文件的目录
    if (strlen(starg.localpath) == 0)
    {
        logfile.Write("Localpath is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname, 100); // 本地存放文件的目录
    if (strlen(starg.matchname) == 0)
    {
        logfile.Write("Matchname is null.\n");
        return false;
    }



    GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype); // 上传文件后客户端的文件的处理方式
    if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3))
    {
        logfile.Write("ptype is error.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "localpathbak", starg.localpathbak, 100); // 列出存放要上传文件名的文件
    if ((starg.ptype == 3) && strlen(starg.localpathbak) == 0)
    {
        logfile.Write("Localpathbak is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename, 300); // 已上传成功文件名
    if ((starg.ptype == 1) && strlen(starg.okfilename) == 0)
    {
        logfile.Write("Okfilename is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout); // 进程的心跳时间
    if (starg.timeout == 0)
    {
        logfile.Write("timeout is null.\n");
        return false;
    }

    GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50); // 进程名字
    if (strlen(starg.pname) == 0)
    {
        logfile.Write("pname is null.\n");
        return false;
    }

    return true;
}

bool _ftpputfiles()
{
    // 把localpath目录下的文件加载到vlistfile2容器中
    if (LoadLocalFile() == false)
    {
        logfile.Write("LoadLocalFile() failed.\n");
        return false;
    }
    PActive.UptATime(); 

    // 处理容器
    if (starg.ptype == 1)
    {
        // 加载okfilename文件中的内容到容器vlistfile1中
        LoadOKFile();

        // 比较vlistfile2和vlistfile1， 得到vlistfile3和 vlistfile4
        CompVector();

        // 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的就okfilename文件
        WriteToOkFile();

        // 把vlistfile4中的内容到vlistfile2中
        vfileinfo2.clear();
        vfileinfo2.swap(vfileinfo4);
    }
    PActive.UptATime();

    // 遍历容器vfilelist
    char strremotefilename[301], strlocalfilename[301];
    for (int ii = 0; ii < vfileinfo2.size(); ii++)
    {
        SNPRINTF(strremotefilename, sizeof(strremotefilename), 300, "%s/%s", starg.remotepath, vfileinfo2[ii].filename);
        SNPRINTF(strlocalfilename, sizeof(strlocalfilename), 300, "%s/%s", starg.localpath, vfileinfo2[ii].filename);
        // 调用ftp.put()方法从传文件到服务器
        logfile.Write("put %s ...", strlocalfilename);
        if (ftp.put(strlocalfilename, strremotefilename, true) == false)
        {
            logfile.WriteEx("failed.\n");
            return false;
        }

        logfile.WriteEx("ok.\n");
        PActive.UptATime();

        // 如果ptype == 1，把上传成功的文件记录追加到okfilenam文件中
        if (starg.ptype == 1)
        {
            AppenedToOKFile(&vfileinfo2[ii]);
        }
        // 删除文件
        if (starg.ptype == 2)
        {
            if (REMOVE(strlocalfilename) == false)
            {
                logfile.Write("REMOVE(%s) failed.\n", strremotefilename);
                return false;
            }
        }
        else if (starg.ptype == 3)
        {
            // 转存到备份文件
            char strlocalfilenamebak[301];
            SNPRINTF(strlocalfilenamebak, sizeof(strlocalfilenamebak), 300, "%s/%s", starg.localpathbak, vfileinfo2[ii].filename);
            if (RENAME(strlocalfilename, strlocalfilenamebak) == false)
            {
                logfile.Write("RENAME(%s, %s) failed.\n", strlocalfilename, strlocalfilenamebak);
                return false;
            }
        }
    }
    return true;
}

bool LoadLocalFile()
{
    vfileinfo2.clear();

    CDir Dir;

    Dir.SetDateFMT("yyyymmddhh24miss");
    // 如果目录下的总文件数超过1000，增量上传文件的功能会有问题
    if(Dir.OpenDir(starg.localpath, starg.matchname) == false){
        logfile.Write("Dir OpenDir(%s) 失败.\n", starg.localpath);
        return false;
    }
    struct st_fileinfo stfileinfo;
    while (true)
    {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
        if(Dir.ReadDir() == false){
            break;
        }
        strcpy(stfileinfo.filename, Dir.m_FileName); // 文件名，不包括目录名
        strcpy(stfileinfo.mtime, Dir.m_ModifyTime); // 文件时间
        vfileinfo2.push_back(stfileinfo);
    }
    // for(int ii = 0; ii < vfileinfo2.size(); ii++){
    //     logfile.Write("filename=%s=\n", vfileinfo2[ii].filename);
    // }
    return true;
}

// 加载okfilename文件中的内容到容器vlistfile1中
bool LoadOKFile()
{
    vfileinfo1.clear();

    CFile File;
    if (File.Open(starg.okfilename, "r") == false)
    {
        // 如果程序是第一次上传，okfilename是不存在的，并不是错误，所以也返回true
        return true;
    }

    char strBuffer[501];

    struct st_fileinfo stfileinfo;
    while (true)
    {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
        if (File.Fgets(strBuffer, 300, true) == false)
        {
            break;
        }
        GetXMLBuffer(strBuffer, "filename", stfileinfo.filename);
        GetXMLBuffer(strBuffer, "mtime", stfileinfo.mtime);
        vfileinfo1.push_back(stfileinfo);
    }
    return true;
}

// 比较vlistfile2和vlistfile1， 得到vlistfile3和 vlistfile4
bool CompVector()
{
    vfileinfo3.clear();
    vfileinfo4.clear();
    int ii, jj;
    for (ii = 0; ii < vfileinfo2.size(); ii++)
    {
        for (jj = 0; jj < vfileinfo1.size(); jj++)
        {
            if ((strcmp(vfileinfo2[ii].filename, vfileinfo1[jj].filename) == 0) &&
                (strcmp(vfileinfo2[ii].mtime, vfileinfo1[jj].mtime) == 0))
            {
                vfileinfo3.push_back(vfileinfo2[ii]);
                break;
            }
        }
        if (jj == vfileinfo1.size())
        {
            vfileinfo4.push_back(vfileinfo2[ii]);
        }
    }
    return true;
}

// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的就okfilename文件
bool WriteToOkFile()
{
    CFile File;
    if (File.Open(starg.okfilename, "w") == false)
    {
        logfile.Write("File Open(%s) failed.\n", starg.okfilename);
        return false;
    }
    for (int ii = 0; ii < vfileinfo3.size(); ii++)
    {
        File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vfileinfo3[ii].filename, vfileinfo3[ii].mtime);
    }
    return true;
}

// 把vlistfile4中的内容到vlistfile2中
bool AppenedToOKFile(struct st_fileinfo *stfileinfo)
{
    CFile File;
    if (File.Open(starg.okfilename, "a") == false)
    {
        logfile.Write("File Open(%s) failed.\n", starg.okfilename);
        return false;
    }
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo->filename, stfileinfo->mtime);
    return true;
}
