/*
 * obtmindtodb.cpp，本程序用于把全国站点参数数据保存到数据库T_ZHOBTCODE表中,支持xml和csv两种文件
 *作者：author
 */
// 表结构
// -- test.T_ZHOBTCODE definition

// CREATE TABLE `T_ZHOBTCODE` (
//   `obtid` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '站点代码',
//   `cityname` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '城市名字',
//   `provname` varchar(30) COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '省名称',
//   `lat` int NOT NULL COMMENT '维度，单位：0.01度',
//   `lon` int NOT NULL COMMENT '经度，单位：0.01度',
//   `height` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '海拔高度，单位0.1米',
//   `upttime` timestamp NOT NULL COMMENT '更新时间，数据被插入或更新的时间',
//   `keyid` int NOT NULL AUTO_INCREMENT COMMENT '记录编号，自动增长列',
//   PRIMARY KEY (`obtid`),
//   UNIQUE KEY `T_ZHOBTCODE_UN` (`keyid`)
// ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='全国站点参数';
#include "_public.h"
#include "_mysql.h"

CFile File;

CPActive PActive;

CLogFile logfile;

// 数据库连接对象
connection conn;

void EXIT(int sig);
bool _obtmindtodb(char *pathname, char *connstr, char *charset);

struct st_zhobtmind
{
    char obtid[11];     // 站点代码。
    char ddatetime[21]; // 数据时间，精确到分钟。
    char t[11];         // 温度，单位：0.1摄氏度。
    char p[11];         // 气压，单位：0.1百帕。
    char u[11];         // 相对湿度，0-100之间的值。
    char wd[11];        // 风向，0-360之间的值。
    char wf[11];        // 风速：单位0.1m/s。
    char r[11];         // 降雨量：0.1mm。
    char vis[11];       // 能见度：0.1米。
} stzhobtmind;

int main(int argc, char *argv[])
{

    // 帮助文档。
    if (argc != 5)
    {
        printf("\n");
        printf("Using:./obtmindtodb pathname connstr charset logfile\n");
        printf("Example:/project/tools1/bin/procctl 10 /project/idc1/bin/obtmindtodb /idcdata/surfdata \"127.0.0.1,root,mysqlpwd,mysql,3306\" utf8 /log/idc/obtmindtodb.log\n\n");
        printf("本程序用于把全国站点分钟观测数据保存到数据库的T_ZHOBTMIND表中，数据只插入，不更新。\n");
        printf("pathname 全国站点分钟观测数据文件存放的目录。\n");
        printf("connstr  数据库连接参数：ip,username,password,dbname,port\n");
        printf("charset  数据库的字符集。\n");
        printf("logfile  本程序运行的日志文件名。\n");
        printf("程序每10秒运行一次，由procctl调度。\n\n\n");
        return -1;
    }

    // 处理程序退出的信号
    // CloseIOAndSignal();
    // signal(SIGINT, EXIT);
    // signal(SIGTERM, EXIT);
    // 打开日志文件
    if (logfile.Open(argv[4], "a+") == false)
    {
        printf("打开日志文件失败(%s).\n", argv[4]);
        return -1;
    }
    // 进程的心跳，10秒足够
    // 调试代码的时候要注释以下

    // PActive.AddPInfo(30,"obtmindtodb");
    // 把全国站点参数文件加载到vstcode容器中
    PActive.AddPInfo(5000, "obtmindtodb");

    _obtmindtodb(argv[1], argv[2], argv[3]);

    return 0;
}

void EXIT(int sig)
{
    logfile.Write("程序退出,sig+%d\n\n", sig);
    conn.disconnect();
    exit(0);
}

bool _obtmindtodb(char *pathname, char *connstr, char *charset)
{
    sqlstatement stmt;

    // 打开目录
    CDir Dir;
    if (Dir.OpenDir(pathname, "*.xml") == false)
    {
        logfile.Write("Dir.OpenDir(%s) failed.\n", pathname);
        return false;
    }

    while (true)
    {
        // 读取目录，得到一个数据文件名
        if (Dir.ReadDir() == false)
        {
            break;
        }
        // 连接数据库
        if (conn.m_state == 0)
        {
            if (conn.connecttodb(connstr, charset) != 0)
            {
                logfile.Write("connect database(%s) failed.\n%s\n", connstr, conn.m_cda.message);
                return -1;
            }

            logfile.Write("connect databse(%s) ok.\n", connstr);
        }
        if (stmt.m_state == 0)
        {
            stmt.connect(&conn);
            stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis) values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9)");
            stmt.bindin(1, stzhobtmind.obtid, 10);
            stmt.bindin(2, stzhobtmind.ddatetime, 14);
            stmt.bindin(3, stzhobtmind.t, 10);
            stmt.bindin(4, stzhobtmind.p, 10);
            stmt.bindin(5, stzhobtmind.u, 10);
            stmt.bindin(6, stzhobtmind.wd, 10);
            stmt.bindin(7, stzhobtmind.wf, 10);
            stmt.bindin(8, stzhobtmind.r, 10);
            stmt.bindin(9, stzhobtmind.vis, 10);
        }

        logfile.Write("filename=%s\n", Dir.m_FileName);
        // 打开文件
        if (File.Open(Dir.m_FullFileName, "r") == false)
        {
            logfile.Write("File.Open(%s) failed.\n", Dir.m_FullFileName);
            return false;
        }
        // 存取从文件中读取的一行
        char strBuffer[1001];
        while (true)
        {
            if (File.FFGETS(strBuffer, 1000, "<endl/>") == false)
            {
                break;
            }
            logfile.Write("strbuffer=%s", strBuffer);
            // 处理文件中的每一行
            memset(&stzhobtmind, 0, sizeof(struct st_zhobtmind));
            GetXMLBuffer(strBuffer, "obtid", stzhobtmind.obtid, 10);
            GetXMLBuffer(strBuffer, "ddatetime", stzhobtmind.ddatetime, 14);
            char tmp[11];
            GetXMLBuffer(strBuffer, "t", tmp, 10);
            if (strlen(tmp) > 0)
                snprintf(stzhobtmind.t, 10, "%d", (int)(atof(tmp) * 10));
            GetXMLBuffer(strBuffer, "p", tmp, 10);
            if (strlen(tmp) > 0)
                snprintf(stzhobtmind.p, 10, "%d", (int)(atof(tmp) * 10));
            GetXMLBuffer(strBuffer, "u", stzhobtmind.u, 10);
            GetXMLBuffer(strBuffer, "wd", stzhobtmind.wd, 10);
            GetXMLBuffer(strBuffer, "wf", tmp, 10);
            if (strlen(tmp) > 0)
                snprintf(stzhobtmind.wf, 10, "%d", (int)(atof(tmp) * 10));
            GetXMLBuffer(strBuffer, "r", tmp, 10);
            if (strlen(tmp) > 0)
                snprintf(stzhobtmind.r, 10, "%d", (int)(atof(tmp) * 10));
            GetXMLBuffer(strBuffer, "vis", tmp, 10);
            if (strlen(tmp) > 0)
                snprintf(stzhobtmind.vis, 10, "%d", (int)(atof(tmp) * 10));
            logfile.Write("obtid=%s,ddatetime=%s,t=%s,p=%s,u=%s,wd=%s,wf=%s,r=%s,vis=%s\n", stzhobtmind.obtid, stzhobtmind.ddatetime, stzhobtmind.t, stzhobtmind.p, stzhobtmind.u, stzhobtmind.wd, stzhobtmind.wf, stzhobtmind.r, stzhobtmind.vis);
            // 把结构体中的数据插入表中
            if (stmt.execute() != 0)
            {
                // 失败的情况有哪些?是否全部写日志
                // 1.记录重复2.数据内容非法
                // 失败了怎么办?是否继续?是否rollback?是否false
                // 答：如果失败的原因是数据内容非法，记录日志后继续：如是是记录重复，不比记录日志，继续
                if (stmt.m_cda.rc != 1062)
                {
                    logfile.Write("BUffer=%s\n", strBuffer);
                    logfile.Write("stmt.execu8te() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
                }
            }
        }
        // 删除文件，提交事务
        // File.CloseAndRemove();
        conn.commit();
    }
    return true;
}