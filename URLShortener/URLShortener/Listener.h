#pragma once

const int BUFFLEN = 2048;

struct Session
{

	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFFLEN] = {};
	int recvBytes = 0;
	char sendBuffer[2048] = {};
	int sendBytes = 0;
};


enum class EventType :int
{
	RECV,
	SEND
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	EventType type;
};

class Listener
{

public:
	Listener();
	~Listener();

public:
	bool Launch();
	bool Accept();

	void ProcessTask(HANDLE handle);
	void ProcessRecv(Session* session);
	void Disconnet(Session* session);


private:
	HANDLE _iocpHandle;
	SOCKET _socket = INVALID_SOCKET;
	SOCKADDR_IN listenerAddr;

	vector<thread>	_threads;
	mutex _mutex;
};
