/* 
 * File:   s_aio.h
 * Author: qgymib
 *
 * Created on 2014年5月13日, 下午1:59
 */

#ifndef S_AIO_H
#define	S_AIO_H

#include "clientbuffer.h"

/**
 * 异步IO所需参数
 */
struct AIO_ATTR
{
    int fd;
    void (*callback)(void* ptr);
    
    struct ClientBuffer* clientBuffer;

    char* recvBuffer;
    int recvBufferSize;
    int recvResult;

    char* sendBuffer;
    int sendSize;
    int sendResult;
    
    int hasCache;
};

/**
 * AIO-读取Socket
 * @param ptr
 * @return 
 */
int AIO_ReadSocket(struct AIO_ATTR *ptr);
/**
 * AIO-写入Socket
 * @param ptr
 * @return 
 */
int AIO_WriteSocket(struct AIO_ATTR *ptr);
/**
 * 初始化AIO模块
 */
void AIO_Init();
/**
 * 终止AIO模块，此方法为阻塞的
 */
void AIO_CleanUp();

#endif	/* S_AIO_H */

