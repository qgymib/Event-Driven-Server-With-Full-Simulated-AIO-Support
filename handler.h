/* 
 * File:   handler.h
 * Author: qgymib
 *
 * Created on 2014年6月13日, 上午9:54
 */

#ifndef HANDLER_H
#define	HANDLER_H

/**
 * 异步读取Socket回调函数
 * @param ptr
 */
void callback_readSocket(void *ptr);
/**
 * 异步写入Socket回调函数
 * @param ptr
 */
void callback_writeSocket(void* ptr);
/**
 * 客户端读事件处理
 * @param ptr
 */
void handleClient(struct AIO_ATTR* ptr);
/**
 * 包裹指定信息
 * @param attr
 * @param bsize
 * @param type
 * @param message
 * @return 
 */
int packageMessage(struct AIO_ATTR* attr, int bsize, int type, const char* message);

#endif	/* HANDLER_H */

