#pragma once
#include <atomic>
class MYSQL;

/**
 * \brief
 * DB URL쇼트너는 DB를 계속 봐야한다.
 * 커넥트를 반복하는 것은 오버헤드가 클 것이다
 * Listener 생성자에서 커넥트를 하고 사용하자
 * 동기화 문제는 접근시 락을 걸까?
 */

void Test();
class DBConnector
{
public:

	DBConnector();
	~DBConnector();


	void SendTest();


	void SearchLongURL_Query(string url);

	static void GetShortURL_Index(unsigned __int64& /*OUT*/index);

	//TLS로 범위 지정해줘서 하는 방법도 있지만... 후순위
	static atomic<unsigned __int64> shortIndex;

private:

private:
	
	MYSQL* Conn=nullptr;                 // MySQL 정보를 담을 구조체
	MYSQL* ConnPtr = nullptr;
};
