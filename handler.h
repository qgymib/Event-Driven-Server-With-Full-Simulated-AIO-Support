/* 
 * File:   handler.h
 * Author: qgymib
 *
 * Created on 2014年6月13日, 上午9:54
 */

#ifndef HANDLER_H
#define	HANDLER_H

void callback_readSocket(void *ptr);
void callback_writeSocket(void* ptr);

void handleClient(struct AIO_ATTR* ptr);

int packageMessage(struct AIO_ATTR* attr, int bsize, int type, const char* message);

#endif	/* HANDLER_H */

