#include <iostream>
using namespace std;

#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#include "define.h"
#include "tcp.h"

int Socket(int family, int type, int protocol)
{
    int n;
    if ((n = socket(family, type, protocol)) < 0)
    {
        cerr << "Socket Create Failed" << endl;
        if (raise(SIGINT))
        {
#ifdef EXIT_ON_ERROR
            exit(EXIT_FAILURE);
#endif
        }
    }
    return n;
}

int Bind(int socket, const struct sockaddr *address,
         socklen_t address_len)
{
    int ret = bind(socket, address, address_len);
    if (ret == -1)
    {
        cerr << "Server Socket Bind Failed" << endl;
        if (raise(SIGINT))
        {
#ifdef EXIT_ON_ERROR
            exit(EXIT_FAILURE);
#endif
        }
    }

    return ret;
}

int Listen(int socket, int backlog)
{
    int ret = listen(socket, backlog);

    if (ret == -1)
    {
        cerr << "Server Socket Listen Failed" << endl;
        if (raise(SIGINT))
        {
#ifdef EXIT_ON_ERROR
            exit(EXIT_FAILURE);
#endif
        }
    }
    return ret;
}

/**
 * 设置指定套节字为非阻塞并返回旧版本的选项
 * @param fd
 * @return 
 */
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, new_option) < 0)
    {
        cerr << "set nonblock failed on" << fd << endl;
    }
    return old_option;
}

void addfd(int epollfd, int fd, int option, int oneshot)
{//为文件描述符添加事件  
    struct epoll_event event;
    event.data.fd = fd;
    event.events = option | EPOLLET;
    if (oneshot)
    {//采用EPOLLONETSHOT事件
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    //    setnonblocking(fd);
}

void modfd(int epollfd, int fd, int option, int oneshot)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = option | EPOLLET;
    if (oneshot)
    {//采用EPOLLONETSHOT事件
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void delfd(int epollfd, int fd)
{
    int option = fcntl(fd, F_GETFL);
    struct epoll_event event;
    event.data.fd = fd;
    event.events = option;

    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
}

void reset_oneshot(int epollfd, int fd)
{//重置事件 
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

