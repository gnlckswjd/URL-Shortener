#include  "pch.h"

#include "Listener.h"


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
        return 0;
    }

    if(!t_listener.Accept())
    {
        return 0;
    }
    
    WSACleanup();


    return 0;


}