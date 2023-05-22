#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024

int main()
{
    key_t key = 1234;
    int shmid;
    char *shmaddr;
    char *msg = "Hello, shared memory!";

    // 创建共享内存
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // 将共享内存连接到进程地址空间
    if ((shmaddr = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // 写入数据
    strncpy(shmaddr, msg, SHM_SIZE);

    // 从共享内存读取数据
    printf("Message read from shared memory: %s\n", shmaddr);

    // 分离共享内存
    shmdt(shmaddr);

    // 删除共享内存
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}