/*
 *  crtsurfdata1.cpp      用于生成全国气象站点观测的分钟数据
 *  author:leeczxc
*/

#include "_public.h"


int main(int argc, char *argv[]){
    // infile outpath logfile
    if(argc != 4){
        printf("Using:./crtsurfdata1 infile outpath logfile\n");
        printf("Example:/project/idc1/bin/cursurfdata1 /project/idc1/bin/stcode.ini /tmp/surfdata /log/idc");
        printf("infile 全国气象站点参数文件名。\n");
        printf("infile 全国气象站点数据文件存放的目录。\n");
        printf("infile 本程序运行的日志文件名。\n");
        return -1;
    }
    return 0;
}