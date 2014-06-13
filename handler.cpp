#include <boost/crc.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
using namespace boost;

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <sys/epoll.h>

#include "handler.h"
#include "define.h"
#include "s_aio.h"
#include "tcp.h"
#include "clientbuffer.h"
#include "database.h"

extern int epollfd;
extern MYSQL sql_connection;

struct ActionPtr
{
    int type;
    string *message;
    struct AIO_ATTR* aio_attr;
};

static void actionForLogin(struct ActionPtr* ptr);
static void actionForSignup(struct ActionPtr* ptr);
static void actionForSearch(struct ActionPtr* ptr);

void callback_readSocket(void *ptr)
{
    int isVaild = FALSE;
    char* recvBuffer = ((struct AIO_ATTR*) ptr)->recvBuffer;

    if (((struct AIO_ATTR*) ptr)->recvResult == 0)
    {
        cout << "client disconnect" << endl;
        delfd(epollfd, ((struct AIO_ATTR*) ptr)->fd);
        deleteClientBuffer(((struct AIO_ATTR*) ptr)->fd);
        return;
    }

    //储存接受字符串
    string recvMessage(recvBuffer, ((struct AIO_ATTR*) ptr)->recvResult - 1);

    //正则表达式验证
    regex reg(REGEX_PARCEL);
    bool reg_result = regex_match(recvMessage, reg);
    isVaild = reg_result ? TRUE : FALSE;

    if (isVaild)
    {
        //CRC32验证
        int crc32_position = recvMessage.find("_CRC32:", 0);
        string package(recvMessage.substr(0, crc32_position));
        crc_32_type crc_result;
        crc_result.process_bytes(package.data(), package.length());
        stringstream crcConvertor;
        crcConvertor << hex << crc_result.checksum();
        string crcResult = crcConvertor.str();

        string crc32 = recvMessage.substr(crc32_position + 7);

        isVaild = (crc32 == crcResult) ? TRUE : FALSE;
    }

    cout << "RECV:" + recvMessage << endl;

    if (isVaild)
    {
        cout << "valid success" << endl;
        handleClient((struct AIO_ATTR*) ptr);
    }
    else
    {
        cout << "valid failed" << endl;
        //将通知客户端信息不完整
        packageMessage((struct AIO_ATTR*) ptr, WRITER_BUFFER_SIZE, EV_LOST, NULL);
        //通知epoll关注此socket的写事件
        //        modfd(epollfd, ((struct AIO_ATTR*) ptr)->fd, EPOLLOUT, TRUE);
    }
}

void callback_writeSocket(void* ptr)
{
    //通知epoll关注此socket的读事件
    modfd(epollfd, ((struct AIO_ATTR*) ptr)->fd, EPOLLIN, TRUE);
}

void handleClient(struct AIO_ATTR* ptr)
{
    string recvMessage(((struct AIO_ATTR*) ptr)->recvBuffer, ((struct AIO_ATTR*) ptr)->recvResult - 1);
    int type;
    int crc32_position = recvMessage.find("_CRC32:", 0);
    string message = recvMessage.substr(5, crc32_position - 5);
    stringstream ss;
    ss << recvMessage.substr(3, 4);
    ss >> type;

    cout << "Type:" << type << "\n" << "Message:" << message << endl;

    struct ActionPtr aptr;
    aptr.type = type;
    aptr.message = &message;
    aptr.aio_attr = ptr;

    switch (type)
    {
    case EV_FIN:
        packageMessage(ptr, WRITER_BUFFER_SIZE, EV_FIN, NULL);

        ptr->callback = callback_writeSocket;
        AIO_WriteSocket(ptr);
        break;

    case EV_SEARCH:
        actionForSearch(&aptr);
        break;

    case EV_FIX:
        break;

    case EV_INSERT:
        break;

    case EV_DELETE:
        break;

    case EV_SIGNUP:
        actionForSignup(&aptr);
        break;

    case EV_LOGIN:
        actionForLogin(&aptr);
        break;

    case EV_LOST:
        ptr->callback = callback_writeSocket;
        AIO_WriteSocket(ptr);
        break;

    default:
        break;
    }
}

int packageMessage(struct AIO_ATTR* attr, int bsize, int type, const char* message)
{
    stringstream ss;
    ss << type;

    string send_message("0x0" + ss.str());

    if (message != NULL)
    {
        send_message = send_message + "_" + message;
    }

    crc_32_type crc32;
    crc32.process_bytes(send_message.data(), send_message.length());
    stringstream crcConvertor;
    crcConvertor << hex << crc32.checksum();
    string crcResult = crcConvertor.str();

    send_message += "_CRC32:" + crcResult + "\n";

#ifdef DEBUG
    cout << "V CRC32:" << crcResult << endl;
#endif

    //发送缓冲区需要预留两个字节：\0和\n
    int message_size = send_message.length();
    cout << "Packaged Size:" << message_size << endl;

    if (message_size <= bsize)
    {
        //指定发送数据大小
        attr->clientBuffer->send_size = message_size;
        //将字符串拷贝到发送缓冲区
        strncpy(attr->clientBuffer->sendBuffer, send_message.data(), send_message.length() + 1);

        return TRUE;
    }
    return FALSE;
}

static void actionForLogin(struct ActionPtr* ptr)
{
    string result;

    string *message = ptr->message;
    vector<string> strVec;
    split(strVec, *message, is_any_of("_"));
    vector<string>::iterator it = strVec.begin();

    string username = *it;
    string passwd_md5 = *(it + 1);

#ifdef DEBUG
    cout << "LOGIN: U-" << username << " P-" << passwd_md5 << endl;
#endif

    MYSQL_RES *sql_result;
    MYSQL_ROW row;
    string sql_vaild_user;
    sql_vaild_user.append("SELECT permission FROM `account_info` WHERE username = '")
            .append(username).append("' AND passwd = '").append(passwd_md5).append("'");
    string sql_vaild_username;
    sql_vaild_username.append("SELECT username FROM `account_info` WHERE username = '")
            .append(username).append("'");

    mysql_query(&sql_connection, sql_vaild_user.data());
    sql_result = mysql_store_result(&sql_connection);

    row = mysql_fetch_row(sql_result);
    cout << "line 2" << endl;
    if (row != NULL)
    {
        result = row[0];
        mysql_free_result(sql_result);
    }
    else
    {
        mysql_free_result(sql_result);

        mysql_query(&sql_connection, sql_vaild_username.data());
        sql_result = mysql_store_result(&sql_connection);
        row = mysql_fetch_row(sql_result);

        stringstream ss;
        (row != NULL) ? (ss << ERROR_PASSWD_INVALID) : (ss << ERROR_USERNAME_INVALID);
        result = ss.str();

        mysql_free_result(sql_result);
    }

    packageMessage(ptr->aio_attr, WRITER_BUFFER_SIZE, EV_LOGIN, result.data());

    ptr->aio_attr->callback = callback_writeSocket;
    AIO_WriteSocket(ptr->aio_attr);
}

static void actionForSignup(struct ActionPtr* ptr)
{
    string result;

    string *message = ptr->message;
    vector<string> strVec;
    split(strVec, *message, is_any_of("_"));
    vector<string>::iterator it = strVec.begin();

    string username = *it;
    string passwd_md5 = *(it + 1);

#ifdef DEBUG
    cout << "SIGNUP: U-" << username << " P-" << passwd_md5 << endl;
#endif

    string sql_vaild_username;
    sql_vaild_username.append("SELECT username FROM `account_info` WHERE username = '")
            .append(username).append("'");
    string sql_insert_account;
    sql_insert_account.append("INSERT INTO `find_the_toilet`.`account_info` ")
            .append("(`id` ,`username` ,`passwd` ,`permission`) ")
            .append("VALUES (NULL , '").append(username).append("', '")
            .append(passwd_md5).append("', '").append("0');");

    mysql_query(&sql_connection, sql_vaild_username.data());
    MYSQL_RES *sql_result = mysql_store_result(&sql_connection);
    MYSQL_ROW row = mysql_fetch_row(sql_result);
    mysql_free_result(sql_result);
    stringstream ss;

    if (row != NULL)
    { //用户名已存在
        ss << ERROR_USERNAME_TOKEN;
    }
    else
    { //用户名可用
        mysql_query(&sql_connection, sql_insert_account.data());
        ss << USER_PERMISSION_NORMAL;
        mysql_free_result(sql_result);
    }

    result = ss.str();

    packageMessage(ptr->aio_attr, WRITER_BUFFER_SIZE, EV_SIGNUP, result.data());

    ptr->aio_attr->callback = callback_writeSocket;
    AIO_WriteSocket(ptr->aio_attr);
}

static void actionForSearch(struct ActionPtr* ptr)
{
    string version;
    string value;

    string *message = ptr->message;
    vector<string> strVec;
    split(strVec, *message, is_any_of("_"));
    vector<string>::iterator it = strVec.begin();

    string location_key = *it;

    string sql_search;
    sql_search.append("SELECT version, value FROM `location` WHERE location_key = '")
            .append(location_key).append("'");

    mysql_query(&sql_connection, sql_search.data());
    MYSQL_RES *sql_result = mysql_store_result(&sql_connection);
    MYSQL_ROW row = mysql_fetch_row(sql_result);
    mysql_free_result(sql_result);

    if (row != NULL)
    {//信息存在
        version = row[0];
        value = row[1];
        packageMessage(ptr->aio_attr, WRITER_BUFFER_SIZE, EV_SEARCH_VALUE, value.data());
    }
    else
    {//信息不存在
        version = "-1";
        value = "";
        packageMessage(ptr->aio_attr, WRITER_BUFFER_SIZE, EV_SEARCH_VERSION, version.data());
    }

    ptr->aio_attr->callback = callback_writeSocket;
    AIO_WriteSocket(ptr->aio_attr);
}
