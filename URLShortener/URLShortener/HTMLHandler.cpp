#include "pch.h"
#include "HTMLHandler.h"
#include "Listener.h"


bool HTMLHandler::HandlePacket(Session* session, string url)
{
	if(url == "/")
	{
		if(SendMainMsg(session))
			return true;
		return false;
	}

	//TODO: /?url=어쩌구저쩌구 DB 검색 후 없으면 쇼트닝
	if(url.find("/?url=")==0)
	{

		//url 유효 체크

		//검색 DB 검색

		//없다? 쇼트닝

		//있다? 가져옴

		//쇼트닝url 정보 전송
		SendShorteningResultMsg();
	}
	else
	{
		return false;
	}

	//TODO: /abcdefgh DB 검색후 없으면 오류, 있으면 리디렉션
	

	return false;
}

bool HTMLHandler::SendMainMsg(Session* session)
{
	const char* msg = "HTTP/1.0 200 OK\r\n"
		"Content-Length: 200\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<html>\n<head>\n<link rel=\"icon\" href=\"data:, \">\n</head>"
		"\n<body>\n<form action=\"\" name=\"urlForm\" method=\"get\">\n    <p>URL: <input type=\"text\" name=\"url\" /></p>\n    <input type=\"submit\" value=\"send\" />\n</form>\n</body>\n"
		"</html>";
	memset(session->sendBuffer, 0, sizeof(BUFFLEN));
	memcpy(session->sendBuffer, msg, strlen(msg));

	WSABUF wsaBuf;
	wsaBuf.buf = session->sendBuffer;
	wsaBuf.len = (ULONG)BUFFLEN;
	DWORD numOfBytes = 0;
	OverlappedEx* overlappedex = new OverlappedEx();
	overlappedex->type = EventType::SEND;

	WSASend(session->socket, &wsaBuf, 1, OUT & numOfBytes, 0, reinterpret_cast<LPWSAOVERLAPPED>(&overlappedex->overlapped), nullptr);
	int errorCode = ::WSAGetLastError();
	if (errorCode != WSA_IO_PENDING)
	{
		if (errorCode != 0)
			return false;
	}
	return true;
}

bool HTMLHandler::SendShorteningResultMsg()
{
	return true;
}

bool HTMLHandler::SendRedirectionMsg()
{
	return true;
}
