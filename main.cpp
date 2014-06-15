/* 
 * File:   main.cpp
 * Author: qgymib
 *
 * Created on 2014年3月21日, 上午8:13
 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

#include <netinet/in.h>
#include <sys/epoll.h>
#include <signal.h>

#include "define.h"
#include "s_aio.h"
#include "tcp.h"
#include "database.h"
#include "handler.h"
#include "clientbuffer.h"

int epollfd; //epoll描述符

static int isRunnable = TRUE;

void signal_termal(int signo);

/*
 * 
 */
int main(int argc, char** argv)
{
    signal(SIGINT, signal_termal);
    
    //初始化异步IO模块
    AIO_Init();
    
    //初始化数据库支持
    initDatabaseConnection();

    int listenfd; //服务器监听套节字
    struct sockaddr_in servaddr; //服务器监听结构
    struct epoll_event events[EPOLL_MAX_EVENT_Q]; //epoll最大并发事件队列

    //创建监听端口
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    //初始化服务器结构
    memset(&servaddr,0 , sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    //绑定端口
    Bind(listenfd, (struct sockaddr*) &servaddr, sizeof (servaddr));

    //监听端口
    Listen(listenfd, LISTENQ);

    epollfd = epoll_create(1);

    //不能将监听端口listenfd设置为EPOLLONESHOT否则会丢失客户连接
    addfd(epollfd, listenfd, EPOLLIN, FALSE);

    cout << "Listen on Port " << SERV_PORT << endl;

    while (isRunnable)
    {
        int ret = epoll_wait(epollfd, events, EPOLL_MAX_EVENT_Q, -1);
        if (ret < 0)
        {
            cout << "no event exist in epoll queue" << endl;
            continue;
        }
        
        for (int i = 0; i < ret; i++)
        {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd)
            { //有新客户端接入
#ifdef DEBUG
                cout << "Accept Event FD:" << events[i].data.fd << endl;
#endif
                
                struct sockaddr_in client_address;
                socklen_t client_addr_length = sizeof (client_address);
                int connfd = accept(listenfd, (struct sockaddr*) &client_address, &client_addr_length);
                setnonblocking(connfd);
                addfd(epollfd, connfd, EPOLLIN, TRUE);
            }
            else if (events[i].events & EPOLLIN)
            { //客户端读事件
#ifdef DEBUG
                cout << "Read Event FD:" << events[i].data.fd << endl;
#endif
                
                struct AIO_ATTR aio_attr;
                aio_attr.fd = events[i].data.fd;
                aio_attr.callback = callback_readSocket;

                struct ClientBuffer *clientBuffer = getClientBuffer(aio_attr.fd);
                if (clientBuffer == NULL)
                {
                    clientBuffer = (struct ClientBuffer*) malloc(sizeof (struct ClientBuffer));
                    clientBuffer->fd = aio_attr.fd;
                    insertClientBuffer(clientBuffer);
                }

                aio_attr.clientBuffer = clientBuffer;
                aio_attr.recvBuffer = clientBuffer->recvBuffer;
                aio_attr.recvBufferSize = READER_BUFFER_SIZE;
                aio_attr.sendBuffer = clientBuffer->sendBuffer;
                aio_attr.hasCache = FALSE;

                //异步IO读取
                AIO_ReadSocket(&aio_attr);
            }
        }
    }

    return (EXIT_SUCCESS);
}

void signal_termal(int signo)
{
    if (signo == 2)
    {
        cout << "Exiting Server..." << endl;

        //拒绝接收新连接
        isRunnable = FALSE;

        //清除异步IO任务
        AIO_CleanUp();
        
        //关闭数据库
        closeDatabaseConnection();

        //删除所有缓存
        freeClientBuffer();

        //退出服务器
        exit(EXIT_SUCCESS);
    }
}
