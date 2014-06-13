#include <cstdlib>
#include <iostream>
using namespace std;

#include <signal.h>

#include "database.h"
#include "define.h"

MYSQL sql_connection;
static bool isConnected = false;

void initDatabaseConnection()
{
    if (mysql_real_connect(&sql_connection, SQL_SERVADDR, SQL_USERNAME, SQL_PASSWD,
                           SQL_DB_NAME, SQL_PORT, NULL, 0))
    {
#ifdef DEBUG
        cout << "Database Connected" << endl;
#endif
        isConnected = true;
    }
    else
    {
        cerr << "Database Connect Failed" << endl;
        isConnected = false;

        if (raise(SIGINT))
        {
#ifdef EXIT_ON_ERROR
            exit(EXIT_FAILURE);
#endif
        }
    }
}

void closeDatabaseConnection()
{
    if (isConnected)
    {
        mysql_close(&sql_connection);
    }
}
