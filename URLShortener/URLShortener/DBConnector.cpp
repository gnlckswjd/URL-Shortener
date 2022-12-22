#include "pch.h"
#include "DBConnector.h"
#include <format>


#include <mysql.h>
#pragma  comment(lib, "libmySQL.lib")


atomic<unsigned __int64> DBConnector::shortIndex = 0;

void Test()
{


    printf("%s\n", mysql_get_client_info());

    MYSQL Conn;                 // MySQL 정보를 담을 구조체
    MYSQL* ConnPtr = NULL;      // MYSQL 핸들
    MYSQL_RES* Result;          // 쿼리성공시 결과를 담는 구조체 포인터
    MYSQL_ROW ROW;              // 쿼리성공시 결과로 나온 행의 정보를 담는 구조체
    int Stat;                   // 쿼리요청 후 결과(성공, 실패)

    mysql_init(&Conn); //MySQL 정보 초기화

    //데이터베이스와 연결
    ConnPtr = mysql_real_connect(&Conn, "localhost", "root", "1234", "mydb", 3306, NULL, 0);

    if (ConnPtr == NULL)
    {
        cout << "Connection Error" << endl;
        return;
    }

    const char* Query = "SELECT * FROM urlinfo";
    Stat = mysql_query(ConnPtr, Query);
    if (Stat != 0)
    {
        cout << "Query Error" << endl;
    }
    Result = mysql_store_result(ConnPtr);

    while ((ROW = mysql_fetch_row(Result)) != NULL)
        cout << ROW[0] << " " << ROW[1] << endl;
    mysql_free_result(Result);


    Query = "SELECT * FROM urlinfo";
    Stat = mysql_query(ConnPtr, Query);
    if (Stat != 0)
    {
        cout << "Query Error" << endl;
    }
    Result = mysql_store_result(ConnPtr);

    while ((ROW = mysql_fetch_row(Result)) != NULL)
        cout << ROW[0] << " " << ROW[1] << endl;
    mysql_free_result(Result);


    mysql_close(ConnPtr);
}

DBConnector::DBConnector()
{
    Conn = new MYSQL();

    mysql_init(Conn);

    ConnPtr = mysql_real_connect(Conn, "localhost", "root", "1234", "mydb", 3306, NULL, 0);
    
}

DBConnector::~DBConnector()
{
    if(ConnPtr != nullptr)
		mysql_close(ConnPtr);

    if (Conn != nullptr)
        delete Conn;

    if (ConnPtr != nullptr)
        delete ConnPtr;
}

void DBConnector::SearchLongURL_Query(string url)
{
    MYSQL_RES* Result;          // 쿼리성공시 결과를 담는 구조체 포인터
    MYSQL_ROW ROW;              // 쿼리성공시 결과로 나온 행의 정보를 담는 구조체
    int Stat;                   // 쿼리요청 후 결과(성공, 실패)

    unsigned long long index;
	GetShortURL_Index(index);

    string query = format("INSERT INTO url_info (short_url,long_url)"
        "SELECT {0}, '{1}' FROM dual WHERE NOT EXISTS"
        "(SELECT * FROM url_info WHERE `long_url` = '{1}');", index, url);
    
    Stat = mysql_query(ConnPtr, query.c_str());
    if (Stat != 0)
    {
        cout << "Query Error" << mysql_error(Conn) << endl;
        //TODO: 처리

        return;
    }
    string query2 = format("SELECT short_url FROM url_info where long_url = '{}';",url);

    Stat = mysql_query(ConnPtr, query2.c_str());
    if (Stat != 0)
    {
        cout << "Query Error" << mysql_error(Conn) << endl;
        //TODO: 처리

        return;
    }
    Result = mysql_store_result(ConnPtr);

    while ((ROW = mysql_fetch_row(Result)) != NULL)
        cout << ROW[0] <<endl;
    mysql_free_result(Result);
}



void DBConnector::SendTest()
{
    MYSQL_RES* Result;          // 쿼리성공시 결과를 담는 구조체 포인터
    MYSQL_ROW ROW;              // 쿼리성공시 결과로 나온 행의 정보를 담는 구조체
    int Stat;                   // 쿼리요청 후 결과(성공, 실패)

    const char* Query = "SELECT * FROM url_info";
    Stat = mysql_query(ConnPtr, Query);
    if (Stat != 0)
    {
        cout << "Query Error" << mysql_error(Conn)<<endl;
    }
    Result = mysql_store_result(ConnPtr);

    while ((ROW = mysql_fetch_row(Result)) != NULL)
        cout << ROW[0] << " " << ROW[1] << endl;
    mysql_free_result(Result);
}

void DBConnector::GetShortURL_Index(unsigned long long& index)
{
    index = shortIndex.fetch_add(1);
}
