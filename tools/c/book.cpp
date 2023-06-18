#include "_public.h"

#define MAXNUMP_ 1000       // 最大的进程数量
#define SHMKEYP_ 0x5095     // 共享内存的key
#define SEMKEYP_ 0x5095     // 信号量key
struct st_pinfo{
    int     pid;        // 进程号
    char    pname[51];  // 进程名字， 可以为空
    int     timeout;    // 超时时间， 单位：秒
    time_t  atime;      // 最后一次心跳时间，用整数表示
};

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Using :./book procname\n");
        return 0;
    }
    // 创建/获取共享内存，大小为n*sizeof(struct st_pinfo)
    int m_shmid = 0;
    if((m_shmid = shmget(SHMKEYP_ , MAXNUMP_ * sizeof(st_pinfo), 0640 | IPC_CREAT)) == -1){
        printf("shmget(%x) failed\n", SHMKEYP_);
        return -1;
    }
    CSEM m_sem;
    if((m_sem.init(SEMKEYP_)) == false){
        printf("m_set.init(%x) failed\n", SEMKEYP_);
        return -1;
    }
    // 将共享内存连接到当前进程的地址空间
    struct st_pinfo *m_shm;
    m_shm = (struct st_pinfo*)shmat(m_shmid,0, 0);
    // 创建当前进程心跳信息结构体变量，把本进程的信息填进去
    struct st_pinfo stpinfo;
    memset(&stpinfo, 0, sizeof(struct st_pinfo));
    stpinfo.pid = getpid();
    STRNCPY(stpinfo.pname, sizeof(stpinfo.pname), argv[1], 50);     // 进程名称
    stpinfo.timeout = 30;                                           // 超时时间
    stpinfo.atime = time(0);

    m_sem.P();                                    // 最后一次心跳的时间
    // 在共享内存中查找一个空位置，把当前进程的心跳信息存入共享内存中
    int m_pos = -1;
    // 如果共享内存中存在当前进程编号，一定是其他进程残留的数据，当前进程就重用改位置
    for(int ii = 0; ii < MAXNUMP_; ii++){
        if(m_shm[ii].pid == stpinfo.pid){
            m_pos == ii;
            break;
        }
    }
    if(m_pos == -1){
        for(int ii = 0; ii < MAXNUMP_; ii++){
            if(m_shm[ii].pid == 0){
                // 找个一个空位置
                m_pos = ii;
                break;
            }
        }
    }


    if(m_pos == -1){
        m_sem.V();
        printf("共享内存空间已用完.\n");
        return -1;
    }

    // 把当前进的心跳信息存入共享内存的进程组中
    memcpy(m_shm + m_pos, &stpinfo, sizeof(struct st_pinfo));

    m_sem.V();
    while(true){
        // 更新共享内存中本进程的心跳时间
        m_shm[m_pos].atime = time(0);
        sleep(10);
    }
    // 把当前进程从共享内存中移去
    // m_shm[m_pos].pid = 0;
    memset(m_shm + m_pos, 0, sizeof(struct st_pinfo));
    // 把共享内存从当前进程中分离
    shmdt(m_shm);
    return 0;
}