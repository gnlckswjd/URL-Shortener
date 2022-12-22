#include "pch.h"
#include "Listener.h"

#include "HTMLHandler.h"
#include "HTTPParser.h"
#include "DBConnector.h"

Listener::Listener() 
{
	DB_Connector = new DBConnector();
}

Listener::~Listener()
{
	if (_socket != INVALID_SOCKET)
		::closesocket(_socket);
	_socket = INVALID_SOCKET;

	WSACleanup();

	if(DB_Connector!=nullptr)
	{
		delete DB_Connector;
		DB_Connector = nullptr;
	}

	for (auto i = _threads.begin(); i != _threads.end(); i++)
	{
		if (i->joinable())
			i->join();
	}
}

bool Listener::Launch()
{
	
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET)
	{
		return false;
	}

	memset(&listenerAddr, 0, sizeof(listenerAddr));
	listenerAddr.sin_family = AF_INET;
	listenerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	listenerAddr.sin_port = htons(80);

	if (SOCKET_ERROR == bind(_socket,reinterpret_cast<SOCKADDR*>(&listenerAddr),sizeof(listenerAddr)))
	{
		return false;
	}

	if (SOCKET_ERROR == listen(_socket, SOMAXCONN))
	{
		return false;
	}

	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (_iocpHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	const unsigned int count = thread::hardware_concurrency();

	for (int i =0; i< count; i++)
	{
		_threads.push_back(thread(&Listener::ProcessTask,this,_iocpHandle));
	}

	return true;
}

bool Listener::Accept()
{
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	SOCKET clientSocket = ::accept(_socket, (SOCKADDR*)&clientAddr, &addrLen);
	if (clientSocket == INVALID_SOCKET)
		return false;


	Session* session = new Session();
	session->socket = clientSocket;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), _iocpHandle, reinterpret_cast<ULONG_PTR>(session), 0);


	WSABUF wsaBuf;
	wsaBuf.buf = session->recvBuffer;
	wsaBuf.len = BUFFLEN;

	OverlappedEx* overlappedEX = new OverlappedEx();
	overlappedEX->type = EventType::RECV;
	
	DWORD recvLen = 0;
	DWORD flags = 0;
	if(WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEX->overlapped, NULL)==SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if(errorCode != WSA_IO_PENDING)
		{
			if(errorCode == WSAECONNABORTED)
			{
				if(errorCode!=0)
					Disconnet(session);
			}
			
		}
	}
	
	return true;
}

void Listener::ProcessTask(HANDLE handle)
{
	while (true)
	{
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_iocpHandle, &bytesTransferred,
			(ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

		

		if (ret == FALSE || bytesTransferred == 0)
		{
			// 자원해제
			Disconnet(session);

			continue;
		}

		switch (overlappedEx->type)
		{
		case EventType::RECV:
			ProcessRecv(session);
			break;

		case EventType::SEND:
			//자원 해제, 메모리

			Disconnet(session);
			break;
		}

		delete overlappedEx;
	}
}

void Listener::ProcessRecv(Session* session)
{
	if (session == nullptr)
	{
		return;
	}


	if (HTTPParser::IsValid(session)== false)
	{
		Disconnet(session);
		return;
	}

	string url;

	if(HTTPParser::ParsePacket(session,url)==false)
	{
		Disconnet(session);
		return;
	}

	//TODO: URL에 요청 값 받아서 나중에 DB에도 전달할 수 있게
	// ?url 로 오는 애들은 DB로
	// localhost/123 으로 오는 애들은 db검색 후 리디렉션
	//SendTest(session);

	if(HTMLHandler::HandlePacket(session,url))
	{
		
	}
	else
	{
		Disconnet(session);
	}
	

}



void Listener::Disconnet(Session* session)
{
	//lock_guard<mutex> lockGuard(_mutex);
	if(session ==nullptr)
	{
		return;
	}

	if (session->socket != INVALID_SOCKET)
	{
		closesocket(session->socket);
	}
		

	delete session;
	session = nullptr;
}

void Listener::SendTest(Session* session)
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
			Disconnet(session);
	}
}
