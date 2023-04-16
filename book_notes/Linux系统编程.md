# Linux 系统编程

## 前言

## 文件IO

* 文件名与`inode`节点的关联被称作链接
* 内核为每个进程维护一个打开文件表，由文件描述符索引。打开文件表存储了指向了文件inode的内存拷贝和元数据
  
***

### `open`

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open (const char *name, int flags);
int open (const char *name, int flags, mode_t mode);

O_APPEND 追加模式打开
O_ASYNC  
```
