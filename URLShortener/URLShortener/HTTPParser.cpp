#include "pch.h"
#include "HTTPParser.h"
#include "Listener.h"


//TODO: if (previous == '\r' && current == '\n')    Cumulative 버퍼에 데이터 넣고 \r\n 두번 나오면 끝?
bool HTTPParser::IsValid(Session* session, int& size)
{
	// 받은 데이터 수만큼
	char* previous= session->recvBuffer;
	char* current = session->recvBuffer;

	for(int i=0; i< BUFFLEN; i++)
	{
		current = session->recvBuffer+i;

		if(*current=='\n'&& *previous == '\r')
		{
			size = i+1;
			return true;
		}

		previous=current;
	}

	size = 0;
	return false;
}
