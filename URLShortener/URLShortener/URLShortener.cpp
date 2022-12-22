#include  "pch.h"

#include "Listener.h"
#include "DBConnector.h"

int main()
{
    

    DBConnector db;
    db.SendTest();
    unsigned __int64 a;
    db.GetShortURL_Index(a);
    db.GetShortURL_Index(a);
    db.GetShortURL_Index(a);
    db.GetShortURL_Index(a);
    db.GetShortURL_Index(a);
    
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

    
}