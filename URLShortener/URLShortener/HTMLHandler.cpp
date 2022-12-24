#include "pch.h"
#include "HTMLHandler.h"
#include "Listener.h"
#include "DBConnector.h"
#include "Rebase64.h"
#include <format>

bool HTMLHandler::HandlePacket(Session* session, string token)
{
	if(token == "/")
	{
		if(SendMainMsg(session))
			return true;
		return false;
	}
	
	if(token.find("/?url=")==0)
	{
		string url = token.substr(6);

		if (url.substr(0, 4) != "www.")
		{
			return false;
		}

		string shortURL;

		{

			DBConnector db;
			if(db.SearchLongURL_Query(url, shortURL)==false)
			{
				return false;
			}

		}
		
		string rebaseURL = Rebase64::Encode(shortURL);



		if (SendShorteningResultMsg(session, rebaseURL))
		{
			return true;
		}

	}

	if (token[0] == '/' && token.length()<10)
	{
		string url = Rebase64::Decode(token.substr(1));
		string longURL;
		{
			DBConnector db;
			if (db.SearchShortURL_Query(url, longURL) == false)
			{
				return false;
			}
		}
		if (SendRedirectionMsg(session, longURL))
		{
			return true;
		}
	}

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

bool HTMLHandler::SendShorteningResultMsg(Session* session, const string& url)
{
	string msg = format("HTTP/1.0 200 OK\r\n"
		"Content-Length: 200\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<html>\n<head>\n<link rel=\"icon\" href=\"data:, \">\n</head>"
		"\n<body>localhost/{}\n</body>\n"
		"</html>",  url);


	const char* resultMsg =msg.c_str();
	memset(session->sendBuffer, 0, sizeof(BUFFLEN));
	memcpy(session->sendBuffer, resultMsg, strlen(resultMsg));

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

bool HTMLHandler::SendRedirectionMsg(Session* session, const string& url)
{
	string msg = format("HTTP/1.0 200 OK\r\n"
		"Content-Length: 200\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<html>\n<head>\n<link rel=\"icon\" href=\"data:, \">\n</head>"
		"\n<body>\n<meta http-equiv=\"refresh\" content=\"0.1; url = http://{}\" />\n</body>\n"
		"</html>", url);


	const char* resultMsg = msg.c_str();
	memset(session->sendBuffer, 0, sizeof(BUFFLEN));
	memcpy(session->sendBuffer, resultMsg, strlen(resultMsg));

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
