/*
 *  crtsurfdata2.cpp      用于生成全国气象站点观测的分钟数据
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

int main(int argc, char *argv[]){
    // infile outpath logfile
    printf("argc = %d\n", argc);
    if(argc != 4){
        printf("Using:./crtsurfdata2 infile outpath logfile\n");
        printf("Example:/project/idc1/bin/cursurfdata2 /project/idc1/bin/stcode.ini /tmp/surfdata /log/idc/crtsurfdata1.log\n");
        printf("infile 全国气象站点参数文件名。\n");
        printf("infile 全国气象站点数据文件存放的目录。\n");
        printf("infile 本程序运行的日志文件名。\n");
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
        logfile.Write("=%s=\n", strBuffer);

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

    for(int ii = 0; ii < vstcode.size(); ii++){
        logfile.Write("provname=%s, obtid=%s, obtname=%s, lat=%.2f, lon=%.2f, height=%.2f\n",
        vstcode[ii].provname, vstcode[ii].obtid, vstcode[ii].obtname, vstcode[ii].lat, vstcode[ii].lon, vstcode[ii].height);
    }
    return true;
}