#include "_ftp.h"

Cftp ftp;

int main(int atgc, char* argv[]){
    if(ftp.login("localhost:21", "Leeczxc", "alichao1") == false){
        printf("localhost login failed\n");
        return -1;
    }else{
        printf("localhost login success\n");
    }

    // if(ftp.mtime("/home/leeczxc/project/weatherproject/public/ftpclient.cpp") == false){
    //     printf("ftp /home/leeczxc/project/weatherproject/public failed.\n");
    //     return -1;
    // }else{
    //     printf("find /home/leeczxc/project/weatherproject/public/ftpclient.cpp, mtime=%s\n", ftp.m_mtime);
    // }

    // if(ftp.size("/home/leeczxc/project/weatherproject/public/ftpclient.cpp") == false){
    //     printf("ftp /home/leeczxc/project/weatherproject/public failed.\n");
    //     return -1;
    // }else{
    //     printf("find ~/project/weatherproject/public/ftpclient.cpp, msize=%d", ftp.m_size);
    // }
    // 获取文件夹信息
    // if(ftp.nlist("~/Project/weatherProject/idc2/c", "/home/Leeczxc/Project/weatherProject/tmp/test.lst") == false){
    //     printf("ftp.nlist(~/project/weatherproject/public/ftpclient.cpp) failed.\n");
    //     return -1;
    // }else{
    //     printf("ftp.nlist(/home/project/weatherproject is ok)\n");
    // }
    ftp.logout();
    return 0;
}
