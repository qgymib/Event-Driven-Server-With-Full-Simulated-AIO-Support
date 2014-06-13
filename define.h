/* 
 * File:   define.h
 * Author: qgymib
 *
 * Created on 2014年5月12日, 下午4:18
 */

#ifndef DEFINE_H
#define	DEFINE_H

#define EXIT_ON_ERROR
#define DEBUG

#define TRUE    1
#define FALSE   0

#define THREAD_POOL_SIZE    10

#define SERV_PORT           9876
#define LISTENQ             100
#define EPOLL_MAX_EVENT_Q   1024

#define SQL_SERVADDR    "127.0.0.1"
#define SQL_PORT        3306
#define SQL_USERNAME    "root"
#define SQL_PASSWD      ""
#define SQL_DB_NAME     "find_the_toilet"

#define READER_BUFFER_SIZE 1024
#define WRITER_BUFFER_SIZE 1024

#define ERROR_THREADPOOL_FAILURE    -1
#define ERROR_THREADPOOL_OVERLOADED -2

#define ERROR_USERNAME_INVALID  -1
#define ERROR_PASSWD_INVALID    -2
#define ERROR_CONNECT           -3
#define ERROR_UNKNOW            -4
#define ERROR_USERNAME_TOKEN    -5

#define EV_FIN      0
#define EV_SEARCH   1
#define EV_FIX      2
#define EV_INSERT   3
#define EV_DELETE   4
#define EV_SIGNUP   5
#define EV_LOGIN    6
#define EV_LOST     7
#define EV_SEARCH_VERSION   8
#define EV_SEARCH_VALUE     9

#define USER_PERMISSION_NORMAL      0
#define USER_PERMISSION_ADMIN       1
#define USER_PERMISSION_DEVELOPER   2

#define REGEX_PARCEL "^0x0[0-9][\\d\\w_\\-]*_CRC32:[a-fA-F\\d]{1,}$"

#endif	/* DEFINE_H */

