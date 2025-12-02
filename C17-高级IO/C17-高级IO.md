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

`epoll();` Linux方言

```c
//  epoll - I/O event notification facility
#include <sys/epoll.h>

//  open an epoll file descriptor
int epoll_create(int size); 

//  control interface for an epoll file descriptor
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event); 

//  wait for an I/O event on an epoll file descriptor
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);

typedef union epoll_data {
    void    *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;    /* Epoll events */
    epoll_data_t data;      /* User data variable */
};
```

## 其他读写函数

`readv()`、`writev()`

```c
//  read or write data into multiple buffers
#include <sys/uio.h>

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

struct iovec {
    void  *iov_base;    /* Starting address */
    size_t iov_len;     /* Number of bytes to transfer */
};
```

## 存储映射IO

`mmap()`

```c
//  mmap, munmap - map or unmap files or devices into memory
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);

int munmap(void *addr, size_t length);
```

## 文件锁

