#include  "pch.h"

#include "Listener.h"
int main()
{
    /*
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
    {
        return 0;
    }


    Listener t_listener;
    if(!t_listener.Launch())
    {
        return 0;
    }

    while (true)
    {

        if (!t_listener.Accept())
        {
            return 0;
        }
    }
    
    WSACleanup();


    return 0;

    */

    printf("%s\n", mysql_get_client_info());

    MYSQL Conn;                 // MySQL 정보를 담을 구조체
    MYSQL* ConnPtr = NULL;      // MYSQL 핸들
    MYSQL_RES* Result;          // 쿼리성공시 결과를 담는 구조체 포인터
    MYSQL_ROW ROW;              // 쿼리성공시 결과로 나온 행의 정보를 담는 구조체
    int Stat;                   // 쿼리요청 후 결과(성공, 실패)

    mysql_init(&Conn); //MySQL 정보 초기화

    //데이터베이스와 연결
    ConnPtr = mysql_real_connect(&Conn, "localhost", "root", "1234", "mydb", 3306, NULL, 0);

    if(ConnPtr == NULL)
    {
        cout << "Connection Error" << endl;
        return 0;
    }

    const char* Query = "SELECT * FROM urlinfo";
    Stat = mysql_query(ConnPtr, Query);
    if(Stat !=0)
    {
        cout << "Query Error" << endl;
    }
    Result = mysql_store_result(ConnPtr);

    while ((ROW = mysql_fetch_row(Result)) != NULL)
        cout << ROW[0] << " " << ROW[1] << endl;
    mysql_free_result(Result);
    

    mysql_close(ConnPtr);
}