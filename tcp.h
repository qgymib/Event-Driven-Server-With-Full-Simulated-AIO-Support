/* 
 * File:   tcp.h
 * Author: qgymib
 *
 * Created on 2014年4月10日, 下午4:23
 */

#ifndef TCP_H
#define	TCP_H
/**
 * socket包裹函数
 * @param family
 * @param type
 * @param protocol
 * @return 
 */
int Socket(int family, int type, int protocol);
/**
 * bind包裹函数
 * @param socket
 * @param address
 * @param address_len
 * @return 
 */
int Bind(int socket, const struct sockaddr *address,
         socklen_t address_len);
/**
 * listen包裹函数
 * @param socket
 * @param backlog
 * @return 
 */
int Listen(int socket, int backlog);
/**
 * 为文件描述符添加事件
 * @param epollfd
 * @param fd
 * @param oneshot
 */
void addfd(int epollfd, int fd, int option, int oneshot);

/**
 * 为文件描述符修改事件
 * @param epollfd
 * @param fd
 * @param option
 */
void modfd(int epollfd, int fd, int option, int oneshot);

/**
 * 删除文件描述符
 * @param epollfd
 * @param fd
 */
void delfd(int epollfd, int fd);

/**
 * 设置文件描述符为非阻塞
 * @param fd
 * @return 
 */
int setnonblocking(int fd);

/**
 * 重置事件
 * @param epollfd
 * @param fd
 */
void reset_oneshot(int epollfd, int fd, int option);

#endif	/* TCP_H */

