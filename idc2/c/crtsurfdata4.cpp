/*
 *  crtsurfdatar.cpp      用于生成全国气象站点观测的分钟数据
 *  author:leeczxc
*/

#include "_public.h"

CLogFile logfile;

// 全国气象站点参数结构体
struct st_stcode{
    char provname[31];  // 省
    char obtid[11];     // 站号
    char obtname[31];   // 站名
    double lat;         // 维度
    double lon;         // 经度
    double height;      // 海拔高度
};

// 存放全国气象站点参数的容器
vector<struct st_stcode> vstcode;

// 把站点参数文件中加载到vstcode容器中
bool loadSTCode(const char *infile);

// 全国气象站点分钟观测数据结构
struct st_surfdata{
    char obtid[11];         // 站点代码
    char ddatetime[21];     // 数据时间：格式yyyymmddhh24miss
    int t;                  // 气温： 单位，0.1摄氏度
    int p;                  // 气压， 0.1百帕
    int u;                  // 相对湿度，0-100之间的值
    int wd;                 // 风向，0-360之间的值
    int wf;                 // 风速： 单位0.1m/s
    int r;                  // 降雨量： 0.1mm
    int vis;                // 能见度：0。1米
};

vector<st_surfdata> vsurfdata; // 存放全国气象站点分钟观测数据的容器

// 存储时间
char strddatetime[21];
// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
void CrtSurfData();

// 把容器vsurfdata中的全国气象站点分钟数据写入文件
bool CrtSurfFile(const char* outpath, const char* datafmt);

int main(int argc, char *argv[]){
    // infile outpath logfile
    printf("argc = %d\n", argc);
    if(argc != 5){
        printf("Using:./crtsurfdata2 infile outpath logfile\n");
        printf("Example:/project/idc1/bin/cursurfdata2 /project/idc1/bin/stcode.ini /tmp/surfdata \
        /log/idc/crtsurfdata1.log xml, json, csv\n");

        printf("infile 全国气象站点参数文件名。\n");
        printf("infile 全国气象站点数据文件存放的目录。\n");
        printf("infile 本程序运行的日志文件名。\n");
        printf("datafmt 数据文件的格式，支持xml、 json和csv三种格式，中间用逗号分隔\n");

        return -1;
    }
    if(logfile.Open(argv[3], "a+", false) == false){
        printf("logfile.Open(%s) failed.\n, argv[3]");
        return -1;
    }

    logfile.Write("crtsurfdata1 开始运行。\n");
    if(loadSTCode(argv[1]) == false){
        return -1;
    }

    // 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中
    CrtSurfData();
    printf("%s", argv[4]);
    // 把容器vsurfdata中的全国气象站点分钟数据写入文件
    if(strstr(argv[4], "xml") != 0){
        CrtSurfFile(argv[2], "xml");
    }
    if(strstr(argv[4], "json") != 0){
        CrtSurfFile(argv[2], "json");
    }
    if(strstr(argv[4], "csv") != 0){
        CrtSurfFile(argv[2], "csv");
    }

    logfile.Write("crtsurfdata1 运行结束。\n");
    return 0;
}

bool loadSTCode(const char *infile){
    // 打开站点参数文件
    CFile File;
    std::cout << infile << std::endl;
    CCmdStr CmdStr;

    struct st_stcode stcode;
    if(File.Open(infile, "r") == false){
        logfile.Write("File open(%s) failed.\n", infile);
        return false;
    }
    char strBuffer[301];
    while(true){
        // 从站点参数文件中读取一行，如果已经读取完，跳出循环。
        if(File.Fgets(strBuffer, 300, true)==false)
            break;

        // 把读取到的每一行拆分
        CmdStr.SplitToCmd(strBuffer, ",", true);
        if(CmdStr.CmdCount()!=6)
            continue;
        memset(&stcode,0,sizeof(struct st_stcode));
        CmdStr.GetValue(0, stcode.provname, 30);    // 省
        CmdStr.GetValue(1, stcode.obtid, 10);       // 站号
        CmdStr.GetValue(2, stcode.obtname, 30);     // 站名
        CmdStr.GetValue(3, &stcode.lat);            // 纬度
        CmdStr.GetValue(4, &stcode.lon);            // 经度
        CmdStr.GetValue(5, &stcode.height);        // 海拔高度

        // 把站点参数结构体放入站点参数容器
        vstcode.emplace_back(stcode);
        // 关闭容器
    }

    // for(int ii = 0; ii < vstcode.size(); ii++){
    //     logfile.Write("provname=%s, obtid=%s, obtname=%s, lat=%.2f, lon=%.2f, height=%.2f\n",
    //     vstcode[ii].provname, vstcode[ii].obtid, vstcode[ii].obtname, vstcode[ii].lat, vstcode[ii].lon, vstcode[ii].height);
    // }
    return true;
};

void CrtSurfData(){
    // 播随机数种子
    srand(time(0));

    // 获取当前时间，当成观测时间

    memset(strddatetime, 0, sizeof(strddatetime));
    LocalTime(strddatetime, "yyyymmddhh24miss");
    // 遍历气象站点参数的vscode容器
    struct st_surfdata stsurfdata;

    for(int ii = 0; ii < vstcode.size(); ii++){
        // 把随机数填充分钟观测数据的结构体
        memset(&stsurfdata, 0, sizeof(struct st_surfdata));
        strncpy(stsurfdata.obtid, vstcode[ii].obtid, 10);   // 站点代码
        strncpy(stsurfdata.ddatetime, strddatetime, 14);    // 数据时间： 格式yyyymmddhh24miss
        stsurfdata.t=rand()%351;       // 气温：单位，0.1摄氏度
        stsurfdata.p=rand()%265+10000; // 气压：0.1百帕
        stsurfdata.u=rand()%100+1;     // 相对湿度，0-100之间的值。
        stsurfdata.wd=rand()%360;      // 风向，0-360之间的值。
        stsurfdata.wf=rand()%150;      // 风速：单位0.1m/s
        stsurfdata.r=rand()%16;        // 降雨量：0.1mm
        stsurfdata.vis=rand()%5001+100000;  // 能见度：0.1米
        // 把观测数据的结构体放入vsurfdata容器
        vsurfdata.emplace_back(stsurfdata);
    }
    printf("done \n");
}

// 把容器vsurfdata中的全国气象站点分钟数据写入文件
bool CrtSurfFile(const char* outpath, const char* datafmt){
    /*
        正确的文件写入方法：
            （1） 创建临时文件
            （2）往临时文件中写入数据
            （3）关闭临时文件
            （4）把临时文件改名为正式文件
    */
    CFile File;

    // 拼接生成数据的文件名， 例如：SURF_ZH_20210629092200_2254.csv
    char strFileName[301];
    sprintf(strFileName, "%s/SURF_ZG_%s_%d.%s", outpath, strddatetime, getpid(), datafmt);
    // 打开文件
    if(File.OpenForRename(strFileName, "w") == false){
        logfile.Write("File.OpenForRename(%s) failed.\n", strFileName);
        return false;
    }
    // 写入第一行标题
    if(strcmp(datafmt, "csv") == 0){
        File.Fprintf("站点代码，数据时间，气温，气压，相对湿度，风向，风俗，降雨量，能见度\n");
    }
    // 遍历存放观测数据的vsurfdata容器
    for(int ii = 0; ii < vsurfdata.size(); ii++){
        //写入记录
        if(strcmp(datafmt, "csv") == 0){
            File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",
            vsurfdata[ii].obtid, vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
            vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
        }
    }

    // 关闭文件
    File.CloseAndRename();
    logfile.Write("生成数据文件%s成功， 数据时间%s, 记录数%d.\n", strFileName, strddatetime, vsurfdata.size());
    return true;
}
