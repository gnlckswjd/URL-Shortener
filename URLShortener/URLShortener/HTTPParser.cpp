#include "pch.h"
#include "HTTPParser.h"

struct Session
{

	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[2048] = {};
	int recvBytes = 0;
};

//TODO: if (previous == '\r' && current == '\n')    Cumulative 버퍼에 데이터 넣고 \r\n 두번 나오면 끝?
bool HTTPParser::Check(Session* session)
{

	// 받은 데이터 수만큼
	char* previous= session->recvBuffer;
	char* current = session->recvBuffer;

	for(int i=0; i< session->recvBytes; i++)
	{
		current = session->recvBuffer+i;

		if(*current=='\n'&& *previous == '\r')
		{
			//TODO: 첫째 줄 발견! 처리
			
		}

		previous=current;
	}

	return true;
}
