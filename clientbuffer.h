/* 
 * File:   client.h
 * Author: qgymib
 *
 * Created on 2014年5月12日, 下午4:19
 */

#ifndef CLIENTBUFFER_H
#define	CLIENTBUFFER_H

#include "define.h"

struct ClientBuffer
{
    int fd;
    int max_recv_size;
    int send_size;
    char recvBuffer[READER_BUFFER_SIZE];
    char sendBuffer[WRITER_BUFFER_SIZE];
};

/**
 * 取得对应文件描述符的客户端缓冲区结构
 * @param fd socket文件描述符
 * @return 
 */
struct ClientBuffer* getClientBuffer(int fd);

/**
 * 注册客户端文件描述符对应的缓冲区。缓冲区结构中必须包含对应的文件描述符
 * @param buffer 缓冲区
 */
void insertClientBuffer(struct ClientBuffer* buffer);

/**
 * 删除对应于文件描述符的缓冲区
 * @param fd 文件描述符
 */
void deleteClientBuffer(int fd);

/**
 * 清空所有缓冲区
 */
void freeClientBuffer();

#endif	/* CLIENTBUFFER_H */

