#include "_public.h"

CSEM sem;



struct st_pid{
    int pid;        // 进程编号
    char name[51];  // 进程名称
};

int main(int argc, char *argv[]){
    // 共享内存的标志
    int shmid;
    // 获取或者创建共享内存，键值为0x5005

    // 如果信号量已存在，获取信号量；如果信号量不存在，则创建它并出示化为value
    if(sem.init(0x5005) == false){
        printf("sem.init(0x5005) failed\n");
        return -1;
    }

    if((shmid = shmget(0x5005, sizeof(struct st_pid), 0640 | IPC_CREAT)) == -1){
        printf("shmget(0x5005) failed\n");
        return -1;
    }
    // 用于指向共享内存的结构体变量
    struct st_pid* stpid = nullptr;

    // 把共享内存连接到当前进程的地址空间
    if((stpid = (struct st_pid*)shmat(shmid, 0, 0)) == (void*)-1){
        printf("shmat failed\n");
        return -1;
    }

    printf("aaa tim=%d, val=%d\n", time(0), sem.value());
    sem.P();    // 加锁
    printf("bbb tim=%d, val=%d\n", time(0), sem.value());
    printf("pid=%d, name=%s\n", stpid->pid, stpid->name);
    stpid->pid = getpid();
    sleep(10);
    strcpy(stpid->name, argv[1]);
    printf("pid=%d, name=%s\n", stpid->pid, stpid->name);
    printf("ccc tim=%d, val=%d\n", time(0), sem.value());
    sem.V();    // 解锁
    printf("ddd tim=%d, val=%d\n", time(0), sem.value());
    // 把共享内存从当前进程中分离
    shmdt(stpid);
    // 删除共享内存
    // if(shmctl(shmid, IPC_RMID, 0) == -1){
    //     printf("shmctl failed\n");
    //     return -1;
    // }
    return 0;
}