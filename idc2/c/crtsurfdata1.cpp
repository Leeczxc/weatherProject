/*
 *  crtsurfdata1.cpp      用于生成全国气象站点观测的分钟数据
 *  author:leeczxc
*/

#include "_public.h"

CLogFile logfile;

int main(int argc, char *argv[]){
    // infile outpath logfile
    printf("argc = %d\n", argc);
    if(argc != 4){
        printf("Using:./crtsurfdata1 infile outpath logfile\n");
        printf("Example:/project/idc1/bin/cursurfdata1 /project/idc1/bin/stcode.ini /tmp/surfdata /log/idc/crtsurfdata1.log\n");
        printf("infile 全国气象站点参数文件名。\n");
        printf("infile 全国气象站点数据文件存放的目录。\n");
        printf("infile 本程序运行的日志文件名。\n");
        return -1;
    }
    if(logfile.Open(argv[3]) == false){
        printf("logfile.Open(%s) failed.\n, argv[3]");
        return -1;
    }
    logfile.Write("crtsurfdata1 开始运行。\n");
    logfile.Write("crtsurfdata1 运行结束。\n");
    return 0;
}