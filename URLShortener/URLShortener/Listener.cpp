#include "pch.h"
#include "Listener.h"
#include "HTMLHandler.h"
#include "HTTPParser.h"
#include "DBConnector.h"


Listener::Listener() 
{
	
}

Listener::~Listener()
{
	if (_socket != INVALID_SOCKET)
		::closesocket(_socket);
	_socket = INVALID_SOCKET;

	WSACleanup();
	
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

	_sendThread = thread([this]() { SendPacket(); });

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
	{
		return false;
	}

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
	if (WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEX->overlapped, NULL)==SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();

		if (errorCode == WSA_IO_PENDING)
		{
			return true;
		}
		
		if (errorCode == WSAECONNABORTED)
		{
			Disconnet(session);
			return false;
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
		BOOL ret = GetQueuedCompletionStatus(_iocpHandle, &bytesTransferred,(ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			Disconnet(session);
			continue;
		}

		switch (overlappedEx->type)
		{
		case EventType::RECV:
			ProcessRecv(session);
			break;

		case EventType::SEND:
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

	if (HTTPParser::IsValid(session) == false)
	{
		Disconnet(session);
		return;
	}

	string url;

	if (HTTPParser::ParsePacket(session,url)==false)
	{
		Disconnet(session);
		return;
	}

	if (HTMLHandler::HandlePacket(session,url)==true)
	{

		{
			lock_guard<mutex> guard(_mutex);
			_sessions.push_back(session);
		}
		return;
	}


	Disconnet(session);
}

void Listener::Disconnet(Session* session)
{
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

void Listener::SendPacket()
{
	while(true)
	{
		Session* session = PopPacket();
		if(session != nullptr)
		{
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
		else
		{
			this_thread::sleep_for(1ms);
		}
	}
}

Session* Listener::PopPacket()
{
	Session* session;
	lock_guard<mutex> guard(_mutex);
	if(_sessions.empty())
	{
		return nullptr;
	}

	session = _sessions.front();

	_sessions.pop_front();
	return session;


}
