# 高级IO

非阻塞IO——阻塞IO

补充：有限状态机编程

## 非阻塞IO

简单流程：自然流程是结构化的

复杂流程：自然结构不是结构化的

## IO多路转接

`select();`

```c
//  select, pselect, FD_CLR, FD_ISSET, FD_SET, FD_ZERO - synchronous I/O multiplexing
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
/**
 *  nfds：      当前最大的文件描述符+1
 *  readfds：   读集
 *  writefds：  写集
 *  exceptfds： 异常集
 *  timeval：   超时时间（不超时设置的话select会阻塞）
 */

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

`poll();`

```c
//  poll, ppoll - wait for some event on a file descriptor
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};
```

`epoll();`

## 其他读写函数

## 存储映射IO

## 文件锁

