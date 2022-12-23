#include "pch.h"
#include "HTTPParser.h"
#include "Listener.h"

//TODO: if (previous == '\r' && current == '\n')    Cumulative 버퍼에 데이터 넣고 \r\n 두번 나오면 끝?
bool HTTPParser::IsValid(Session* session)
{
	if (session == nullptr)
		return false;
	// 받은 데이터 수만큼
	char* previous= session->recvBuffer;
	char* current = session->recvBuffer;

	for(int i=0; i< BUFFLEN; i++)
	{
		current = session->recvBuffer+i;

		if(*current=='\n'&& *previous == '\r')
		{
			session->recvBytes = i+1;
			return true;
		}

		previous=current;
	}

	session->recvBytes = 0;
	return false;
}

bool HTTPParser::ParsePacket(Session* session, string& token)
{
	if (session == nullptr)
	{
		return false;
	}

	string requestMsg{ session->recvBuffer };

	vector<string> tokens;
	stringstream sstream(requestMsg);

	string temp;
	for (int i = 0; i < 2; i++)
	{
		if (getline(sstream, temp, ' '))
		{
			tokens.push_back(temp);
		}
	}
	token = tokens[1];

	return true;
}