#ifndef MIN_HEAP
#define MIN_HEAP

#include <iostream>
#include <netinet/in.h>
#include <time.h>

#define BUFFER_SIZE 64
class heap_timer;

struct client_data{
    sockaddr_in address;
    int socket;
    char buf[BUFFER_SIZE];
    heap_timer* timer;
};

//定时器类
class heap_timer{
public:
    time_t expire; //定时器生效绝对时间
    void (*cb_func)(client_data* data);
    client_data* user_data;
    heap_timer(int delay){
        expire = time(NULL) + delay;
    }
};

//时间堆类
class time_heap{
private:
    heap_timer** array;     //堆数组
    int capacity;   //堆容量
    int cur_size;   //堆当前定时器的个数
    void percolate_down(int hole);
    void resize() throw(std::exception);
public:
    //构造函数1，初始化容量为cap的空堆
    time_heap(int cap)throw(std::exception):capacity(cap), cur_size(0){
        array = new heap_timer*[capacity]; //创建cap容量的数组，每个数组元素是一个定时器类对象
        if(!array){
           throw std::exception(); 
        }
        for(int i = 0; i < capacity; i++){
            array[i] = NULL; //初始化数组
        }
    }
    //构造函数2, 用已有的数组来初始化堆
    time_heap(heap_timer** init_array, int size, int capacity)
        throw(std::exception):cur_size(size), capacity(capacity){
            if(capacity < size){
                throw std::exception();
            }
            array = new heap_timer*[capacity];
            if(!array){
            throw std::exception(); 
            }
            for(int i = 0; i < capacity; i++){
                array[i] = NULL; //初始化数组
            }
            if(size !=0 ){
                for(int i = 0; i < size; i++){
                    array[i] = init_array[i];
                }
                for(int i = (cur_size - 1)/2; i >=0; i--){
                    percolate_down(i);
                }
            }
    }
    ~time_heap(){
        for(int i = 0; i < cur_size; i++){
            delete array[i];
        }
        delete [] array;
    }

    void add_timer(heap_timer* timer) throw(std::exception);
    void del_timer(heap_timer* timer);
    void pop_timer();
    void tick();
    bool empty() const {return cur_size==0;}
    heap_timer* top() const;
};


void time_heap::add_timer(heap_timer* timer) throw(std::exception){
    if(! timer){
        return;
    }
    if(cur_size >= capacity){
        resize();
    }
    /*新来一个定时器，堆数组加1*/
    int hole = cur_size++;
    int parent = 0;
    /*上浮操作，更新堆*/
    for(; hole > 0; hole = parent){
        //找父亲节点
        parent = (hole-1)/2;
        if(array[parent]->expire <= timer->expire){
            break;
        }
        array[hole] = array[parent];
    }
    array[hole] = timer;
}

void time_heap::del_timer(heap_timer* timer){
    if(!timer){
        return;
    }   
    /*延迟销毁，节省删除定时器造成的开销，但会使得堆数组膨胀*/
    timer->cb_func = NULL;
}

heap_timer* time_heap::top() const {
    if(empty){
        return;
    }
    return array[0];
}
void time_heap::pop_timer(){
    if(empty()){
        return;
    }
    if(array[0]){
        delete array[0];
        array[0] = array[--cur_size];
        percolate_down(0);  //对剩余元素执行下沉操作
    }
}

void time_heap::tick(){
    heap_timer* tmp = array[0];
    time_t cur = time(NULL);
    while(!empty()){
        if(!tmp){
            break;
        }
        //定时器没到期，退出循环
        if(tmp->expire > cur){
            break;
        }
        //到期了，执行定时器中的回调函数
        if(array[0]->cb_func){
            array[0]->cb_func(array[0]->user_data);
        }
        //到时的定时器执行完就拿走，生成新的根
        pop_timer();
        tmp = array[0];
    }
}

//下沉操作，以hole为节点的这个定时器拥有最小堆性质
void time_heap::percolate_down(int hole){
    heap_timer* temp = array[hole];
    int child = 0;
    for(; ((hole*2+1)) <= (cur_size-1); hole =child){
        //上浮找的是父节点，下沉找的是孩子节点
        child = hole*2+1;
        if((child < (cur_size-1)) && (array[child+1]->expire < array[child]->expire)){
            child++;
        }
        if(array[child]->expire < temp->expire){
            array[hole] = array[child];
        }else{
            break;
        }
    }
    array[hole] =temp;
}

//扩容一倍
void time_heap::resize() throw(std::exception){
    heap_timer** temp =new heap_timer* [2*capacity];
    for(int i=0; i< 2*capacity; i++){
        temp[i] =NULL;
    }
    if(!temp){
        throw std::exception();
    }
    capacity = 2*capacity;
    for(int i = 0; i < cur_size; i++){
        temp[i] = array[i];
    }
    delete [] array;
    array = temp;
}
#endif