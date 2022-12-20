#pragma once


const int BUFSIZE = 2048;

// TODO : HTTP 파싱
// 유효한 HTTP인가?
// 유효한 요청인가?
// 첫째 줄에 데이터와 요청이 담겨있고 이것만 활용할 것임
// 첫째 줄 확인 되면 나머지 데이터 날리고
// \r\n\r\n 까지만 받고


class HTTPParser
{
public:
	bool IsValid(class Session* session, int& size);

};

