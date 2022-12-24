#pragma once
#include <atomic>

class MYSQL;


class DBConnector
{
public:

	DBConnector();
	~DBConnector();

	bool SearchLongURL_Query(string url, string& shortURL);
	bool SearchShortURL_Query(string url, string& longURL);

	static void GetShortURL_Index(unsigned __int64& /*OUT*/index);

	static atomic<unsigned __int64> shortIndex;
	static mutex dbMutex;
private:
	void DisconnectDB(MYSQL* ConnPtr);
private:
	
	MYSQL* Conn=nullptr;                 
};
