#include "pch.h"
#include "DBConnector.h"
#include <format>

#include <mysql.h>
#pragma  comment(lib, "libmySQL.lib")

atomic<unsigned __int64> DBConnector::shortIndex = 0;
mutex DBConnector::dbMutex;


DBConnector::DBConnector()
{
    Conn = new MYSQL();

    mysql_init(Conn);
}

DBConnector::~DBConnector()
{
    if (Conn != nullptr)
        delete Conn;
}

void DBConnector::DisconnectDB(MYSQL* ConnPtr)
{
    if (ConnPtr != nullptr)
        mysql_close(ConnPtr);
    ConnPtr = nullptr;
}

bool DBConnector::SearchLongURL_Query(string url, string& shortURL)
{
    MYSQL* ConnPtr = nullptr;
    MYSQL_RES* Result;         
    MYSQL_ROW ROW;             
    int Stat;

    {
        lock_guard<mutex> guard(dbMutex);

        ConnPtr = mysql_real_connect(Conn, "localhost", "root", "1234", "mydb", 3306, NULL, 0);
        if (ConnPtr == nullptr)
        {
            return false;
        }

        unsigned long long index;
        GetShortURL_Index(index);

        string query = format("INSERT INTO url_info (short_url,long_url)"
            "SELECT {0}, '{1}' FROM dual WHERE NOT EXISTS"
            "(SELECT * FROM url_info WHERE `long_url` = '{1}');", index, url);

        Stat = mysql_query(ConnPtr, query.c_str());
        if (Stat != 0)
        {
            cout << "Query Error" << mysql_error(Conn) << endl;

            DisconnectDB(ConnPtr);
            return false;
        }

        string query2 = format("SELECT short_url FROM url_info where long_url = '{}';", url);
        Stat = mysql_query(ConnPtr, query2.c_str());
        if (Stat != 0)
        {
            cout << "Query Error" << mysql_error(Conn) << endl;
            //TODO: 처리
            DisconnectDB(ConnPtr);
            return false;
        }

        Result = mysql_store_result(ConnPtr);
    }

    if (mysql_num_rows(Result) != 1)
    {
        DisconnectDB(ConnPtr);
        return false;
    }

    ROW = mysql_fetch_row(Result);
    mysql_free_result(Result);

    if (ROW == NULL)
    {
        DisconnectDB(ConnPtr);
        return false;
    }
    string tempURL(ROW[0]);
    shortURL = tempURL;

    DisconnectDB(ConnPtr);

    return true;
	
}

bool DBConnector::SearchShortURL_Query(string url, string& longURL)
{
    MYSQL* ConnPtr = nullptr;
    MYSQL_RES* Result;
    MYSQL_ROW ROW;
    int Stat;
    {
        lock_guard<mutex> guard(dbMutex);
        ConnPtr = mysql_real_connect(Conn, "localhost", "root", "1234", "mydb", 3306, NULL, 0);
        if (ConnPtr == nullptr)
        {
            return false;
        }

        string query2 = format("SELECT long_url FROM url_info where short_url = '{}';", url);
        Stat = mysql_query(ConnPtr, query2.c_str());
        if (Stat != 0)
        {
            cout << "Query Error" << mysql_error(Conn) << endl;
            DisconnectDB(ConnPtr);
            return false;
        }
        Result = mysql_store_result(ConnPtr);

    }

    if (mysql_num_rows(Result) != 1)
    {
        DisconnectDB(ConnPtr);
        return false;
    }

    ROW = mysql_fetch_row(Result);
    mysql_free_result(Result);

    if (ROW == NULL)
    {
        DisconnectDB(ConnPtr);
        return false;
    }

    string tempURL(ROW[0]);
    longURL = tempURL;
    DisconnectDB(ConnPtr);
    return true;
}


void DBConnector::GetShortURL_Index(unsigned long long& index)
{
    index = shortIndex.fetch_add(1);
}
