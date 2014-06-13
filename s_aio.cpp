#include <iostream>
#include <cstdlib>
using namespace std;

#include <sys/socket.h>
#include <signal.h>

#include "define.h"
#include "s_aio.h"
#include "threadpool.h"
#include "database.h"

static struct threadpool* pool = NULL;

static void readSocketTask(void* ptr);
static void writeSocketTask(void* ptr);

int AIO_ReadSocket(struct AIO_ATTR *ptr)
{
    return threadpool_add_task(pool, readSocketTask, ptr, FALSE);
}

int AIO_WriteSocket(struct AIO_ATTR *ptr)
{
    return threadpool_add_task(pool, writeSocketTask, ptr, FALSE);
}

void AIO_CleanUp()
{
    if (pool != NULL)
    {
        threadpool_free(pool, TRUE);
    }
}

static void readSocketTask(void* ptr)
{
    int fd = ((struct AIO_ATTR*) ptr)->fd;
    void (*callback)(void *ptr) = ((struct AIO_ATTR*) ptr)->callback;
    char* recvBuffer = ((struct AIO_ATTR*) ptr)->recvBuffer;
    int recvBufferSize = ((struct AIO_ATTR*) ptr)->recvBufferSize;
    
    ((struct AIO_ATTR*) ptr)->recvResult = recv(fd, recvBuffer, recvBufferSize, 0);
    callback(ptr);
}

static void writeSocketTask(void* ptr)
{
    int fd = ((struct AIO_ATTR*) ptr)->fd;
    void (*callback)(void *ptr) = ((struct AIO_ATTR*) ptr)->callback;
    char* sendBuffer = ((struct AIO_ATTR*) ptr)->clientBuffer->sendBuffer;
    int sendSize = ((struct AIO_ATTR*) ptr)->clientBuffer->send_size;
    
#ifdef DEBUG
    cout << "SEND SIZE:" << sendSize << endl;
    cout << "SEND:" << string(sendBuffer, sendSize) << "<-END" << endl;
#endif
    
    ((struct AIO_ATTR*) ptr)->sendResult = send(fd, sendBuffer, sendSize, 0);
    
    callback(ptr);
}

void AIO_Init()
{
    if (pool != NULL)
    {
        return;
    }

    if ((pool = threadpool_init(THREAD_POOL_SIZE)) == NULL)
    {
        cerr << "AIO init Failed" << endl;

        if (raise(SIGINT))
        {
#ifdef EXIT_ON_ERROR
            exit(EXIT_FAILURE);
#endif
        }
    }
    
#ifdef DEBUG
    cout << "AIO init success" << endl;
#endif
}
