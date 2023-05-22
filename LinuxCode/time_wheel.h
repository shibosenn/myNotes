#ifndef WHEEL_TIMER_H
#define WHEEL_TIMER_H

#include<time.h>
#include<netinet/in.h>
#include<stdio.h>

#define BUFFER_SIZE 64
class tw_timer;
/*客户端数据信息存储的结构体*/
struct client_data{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    tw_timer* timer;
};

//定时器类
class tw_timer{
public:
    int rotation;     //记录该定时器在时间轮转多少圈后才生效
    int time_slot;    //记录该定时器属于哪一个槽中
    client_data* user_data;
    tw_timer* next;
    tw_timer* prev;
    void (*cb_fun)(client_data* user_data);

    tw_timer(int rotation, int slot):next(NULL), prev(NULL), rotation(rotation), time_slot(slot){}    
};

//时间轮
class time_wheel{
public:
    time_wheel():cur_slot(0){
        for(int i = 0; i < N; i++){
            slots[i] = 0;   //初始化每一个槽的头结点
        }
    }
    ~time_wheel(){
        //销毁每一个槽中的定时器
        for(int i = 0; i < N; i++){
            tw_timer* tmp = slots[i];
            while (tmp)
            {
                //slots[i]是头结点
                slots[i] = tmp->next;
                delete tmp;
                tmp = slots[i];
            }
        }
    }
    tw_timer* add_timer(int timeout);
    void del_timer(tw_timer* timer);
    void tick();
private:
    static const int N = 60;    //slot的数目
    static const int SI = 1;    //从一个slot到下一个slot所耗费的时间，就是tick
    tw_timer* slots[N];         //N个slot，N个链表，每个定时器指向一个
    int cur_slot;               //时间轮当前所在的slot
};

/*含义：根据定时值timeout创建定时器，然后插入相应slot中*/
tw_timer* time_wheel::add_timer(int timeout){
    if(timeout < 0){
        return NULL;
    }
    int ticks = 0;
    /*根据定时器的超时值timeout计算将在时间轮转动多少个tick后被触发，并将该数存放在ticks中。
    如果不满足一个tick时间，就取最小的tick时间为1，如果大于一个tick就除法，将值存于ticks中*/
    if(timeout < SI){
        ticks = 1;
    }else{
        ticks = timeout / SI;
    }

    int rotation = ticks / N;   //看ticks是否超过了一轮，即时间轮转动多长时间会被触发
    int select_slot = (cur_slot + (ticks % N)) % N;     //待插入的定时器应该放在哪个槽中
    tw_timer* timer = new tw_timer(rotation, select_slot);  //创建一个定时器，在转动rotation后被触发
    /*如果某个slot中没有定时器，则将待插入的定时器作为头结点插入*/
    if(!slots[select_slot]){
        printf("add a new timer!, after %d rounds, at %d\n", rotation, select_slot);
        slots[select_slot] = timer;
    }else{
        //头插法
        timer->next = slots[select_slot];
        slots[select_slot]->prev = timer;
        slots[select_slot] =timer;
    }
    return timer;
}

/*删除目标定时器*/
void time_wheel::del_timer(tw_timer* timer){
    if(!timer){
        return;
    }
    int timer_cur_slot = timer->time_slot;
    /*如果该timer是所在槽的头结点，则要重置一下该槽的头结点*/
    if(timer == slots[timer_cur_slot]){
        slots[timer_cur_slot] = slots[timer_cur_slot]->next;
        //为什么要判断，因为这个时候节点已经变成头结点的下一个了，要判断是是否为空
        if(slots[timer_cur_slot]){
            slots[timer_cur_slot]->prev = NULL;
        }
        delete timer;
    }else{
        timer->prev->next = timer->next;
        if(timer->next){
            timer->next->prev = timer->prev;
        }
        delete timer;
    }
}

//一个tick过后，需要调用该函数使得时间轮向前滚动
void time_wheel::tick(){
    tw_timer* tmp = slots[cur_slot];
    printf("current slot is %d\n", cur_slot);
    while (tmp)
    {
        printf("the wheel need to tick once!\n");
        if(tmp->rotation > 0){
            tmp->rotation--;
            tmp = tmp->next;
        }{
            /*定时器到期，执行任务然后删除定时器*/
            tmp->cb_fun(tmp->user_data);
            if(tmp == slots[cur_slot]){
                printf("in head node, need to delete header in cur_slot!\n");
                slots[cur_slot] = tmp->next;
                delete tmp;
                if(slots[cur_slot]){
                    slots[cur_slot]->prev = NULL;
                }
                tmp = slots[cur_slot];
            }else{
                tmp->prev->next = tmp->next;
                if(tmp->next){
                    tmp->next->prev = tmp->prev;
                }
                tw_timer* tmp2 = tmp->next;
                delete tmp;
                tmp = tmp2;
            }
        }
    }
    cur_slot = cur_slot++ % N;
}

#endif