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

void initDatabaseConnection();
void closeDatabaseConnection();

#endif	/* DATABASE_H */

