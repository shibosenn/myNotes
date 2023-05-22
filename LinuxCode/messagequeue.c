#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 128

struct message {
    long mtype;
    char mtext[MSG_SIZE];
};

int main()
{
    key_t key = 1234;
    int msqid;
    struct message msg = {0};

    // 创建消息队列
    if ((msqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    // 发送消息
    msg.mtype = 1;
    strncpy(msg.mtext, "Hello, message queue!", MSG_SIZE);
    if (msgsnd(msqid, (void *)&msg, sizeof(struct message), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    // 接收消息
    if (msgrcv(msqid, (void *)&msg, sizeof(struct message), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("Message received from message queue: %s\n", msg.mtext);

    // 删除消息队列
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    return 0;
}