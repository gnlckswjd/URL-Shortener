#include  "pch.h"

#include "Listener.h"
#include "DBConnector.h"

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
    {
        return 0;
    }

    Listener t_listener;
    if(!t_listener.Launch())
    {
        WSACleanup();
        return 0;
    }

    while (true)
    {

        if (!t_listener.Accept())
        {
            WSACleanup();
            return 0;
        }
    }
    
    WSACleanup();


    return 0;

    
}