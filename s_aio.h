/* 
 * File:   s_aio.h
 * Author: qgymib
 *
 * Created on 2014年5月13日, 下午1:59
 */

#ifndef S_AIO_H
#define	S_AIO_H

#include "clientbuffer.h"

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
};

int AIO_ReadSocket(struct AIO_ATTR *ptr);
int AIO_WriteSocket(struct AIO_ATTR *ptr);

void AIO_Init();
void AIO_CleanUp();

#endif	/* S_AIO_H */

