/*
 * 程序名: 采用tcp协议，实现文件上传的客户端
  * author: Leeczxc
*/

#include "_public.h"

CTcpClient TcpClient;

bool srv000(); // 心跳
bool srv001(); // 登录

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Using:./tcpputfiles ip port \n Examplae:./tcpputfiles localhost 5006\n\n");
        return -1;
    }
    // 向服务端发送连接请求
    if(TcpClient.ConnectToServer(argv[1], atoi(argv[2])) == false){
        printf("TcpClient.ConnectToServer(%s, %s) failed.\n", argv[1], argv[2]);
        return -1;
    }

    // 登录业务
    if(srv001() == false){
        printf("srv001() failed.\n");
        return -1;
    }

    sleep(3);

    // 我的账户（查询余额）
}

// 心跳
bool srv000(){
    char buffer[1024];
    SPRINTF(buffer, sizeof(buffer), "<srvcode>0</srvcode>");
    printf("发送: %s\n", buffer);
    if(TcpClient.Write(buffer) == false){ // 向服务端发送请求报文
        return false;
    }

    memset(buffer, 0, sizeof(buffer));
    if(TcpClient.Read(buffer) == false){ // 接受服务端的回应报文
        return false;
    }

    return true;
}

bool srv001(){
    char buffer[1024];

    SPRINTF(buffer, sizeof(buffer), "<srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>");
    printf("发送：%s\n", buffer);
    if(TcpClient.Write(buffer) == false){
        return false;
    }

    memset(buffer, 0, sizeof(buffer));
    if(TcpClient.Read(buffer) == false){ // 接受服务端的回应报文
        return false;
    }
    printf("接受:%s\n", buffer);

    // 解析服务器端返回的xml
    int iretcode = -1;
    GetXMLBuffer(buffer, "remote", &iretcode);
    if(iretcode != 0){
        printf("登录失败.\n");
        return false;
    }
    printf("登录成功.\n");
    return true;
}