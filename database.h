/* 
 * File:   database.h
 * Author: qgymib
 *
 * Created on 2014年6月12日, 下午10:35
 */

#ifndef DATABASE_H
#define	DATABASE_H

#include <mysql/mysql.h>

extern MYSQL sql_connection;

/**
 * 初始化数据库连接
 */
void initDatabaseConnection();
/**
 * 关闭数据库连接
 */
void closeDatabaseConnection();

#endif	/* DATABASE_H */

