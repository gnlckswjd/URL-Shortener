#include "pch.h"
#include "HTMLHandler.h"
#include "Listener.h"

#include <sstream>

///TODO: 입력 데이터 확인 후 보낼 데이터 지정해서 전송
///1. 메인 화면
///2. 쇼트닝 요청
///3. 쇼트닝 리디렉션


bool HTMLHandler::HandlePacket(Session* session)
{
	if (session->recvBuffer == nullptr)
	{
		return false;
	}

	string requestMsg{session->recvBuffer} ;

	vector<string> tokens;
	stringstream sstream(requestMsg);

	string token;
	for(int i=0; i<2; i++)
	{
		if(getline(sstream, token,' '))
		{
			tokens.push_back(token);
		}
	}
	cout << tokens[1] << endl;

	return false;
}

void HTMLHandler::SendPakcet()
{
	

	char    szBuf[2048];
	const char* msg = "HTTP/1.0 200 OK\r\n"
		"Content-Length: 200\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<html>\n<head>\n<link rel=\"icon\" href=\"data:, \">\n</head>"
		"\n<body>\n<form action=\"\" name=\"urlForm\" method=\"get\">\n    <p>URL: <input type=\"text\" name=\"url\" /></p>\n    <input type=\"submit\" value=\"send\" />\n</form>\n</body>\n"
		"</html>";
	memcpy(&szBuf, msg, strlen(msg));

	//TODO: send
}
