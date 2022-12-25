# URL-Shortener

### 개요
- URL $\rightarrow$ 8자리 단축
  - 동일한 URL에 대한, 동일한 단축 결과
- 단축된 URL로 접속시 리다이렉션
![](https://velog.velcdn.com/images/oak_cassia/post/67d414e5-295b-45f8-8c53-bf44e5e2b3cd/image.png)
- IOCP 모델, 멀티스레드 활용

- 메모리 관리  
  - 더미 클라이언트로 테스트 했을 때, 서버 메모리 점유 상황
	1. 서버 시작
	2. 리다이렉션 요청 (스레드 100개 0.1초 마다 리다이렉션 요청)
	3. 더미 클라이언트 종료
![](https://velog.velcdn.com/images/oak_cassia/post/d06e406e-4093-4f27-acab-20a1aef181b4/image.png)
테스트 결과: 처리가 지연되지만 작동은 함

### 기술 스택
- C++
  - C++20 format 편해보여 사용
  - winsock2 라이브러리
- MySQL (8.0.31.0)

### 확인 받고 싶은 부분
1. Listener, DBConnector를 제외한 나머지 클래스 static 함수만 멤버로 갖고 있는데 나쁜 방법인가요?
   - 위와 같이 구현한 이유
    - 비슷한 기능 함수 클래스 단위 관리
    - 메모리 관리 용이(객체 생성 안해도 되서)
  ```cpp
class HTTPParser
{
public:
	static bool IsValid(class Session* session);
	static bool ParsePacket(class Session* session, string& token);
};



2. 일반 포인터와 string을 많이 사용했는데 괜찮은 건가요? 더 나은 방법은 뭐가 있을 까요? (shared_ptr 등)
    - ex) 
    ```cpp
  bool HTTPParser::ParsePacket(Session* session, string& token)
 
3. DB에 동시 접근할 때 `mysql_fetch_row()` 함수에서 오류가 생겨 lock을 걸었는데 다른 좋은 방법이 있을 까요?[링크]()
   - ```cpp
  bool DBConnector::SearchShortURL_Query(string url, string& longURL)
{
    ...
    {
        lock_guard<mutex> guard(dbMutex);
       
        ...
    }
    ...
};


4. DBConnector 클래스에서 오류가 발생 하는 이유를 모르겠습니다.(dll 관련?)
   - `mysql_real_connect()` 함수 호출 시

        
   - 발생 환경: 서버 구동 후 많은 요청을 바로 받을 때(위의 더미클라이언트)
   - 발생 안할 때 환경: 브라우저로 요청 한번 하고 더미 클라이언트 실행
   - 오류 이미지![](https://velog.velcdn.com/images/oak_cassia/post/09265966-190f-4590-a5d2-49a3c2e22039/image.png)
   - 위치(가장 아래 줄)
  ```cpp
  bool DBConnector::SearchShortURL_Query(string url, string& longURL)
{
    if (Conn == nullptr)
    {
        return false;
    }
    MYSQL* ConnPtr = nullptr;
    MYSQL_RES* Result;
    MYSQL_ROW ROW;
    int Stat;
    {
        lock_guard<mutex> guard(dbMutex);
        
        ConnPtr = mysql_real_connect(Conn, "localhost", "root", "1234", "mydb", 3306, NULL, 0);
        ...
    };

### 궁금한 점
- HTTP는 헤더의 크기를 미리 알 수 없어 `\r\n`이 나올 때 까지 읽었는데 맞는 접근인지

### 개선할 점
- TCP는 쪼개져서 오기 때문에 HTTP 패킷이 온전히 도착했는지 확인해야 한다.
  - 지금은 첫번 째 줄만 정확히 왔는지 확인하고 코드 진행([링크]())

- 리팩토링

- html 개선
-  accept 함수도 IOCP에 맞게 변경
   - 현재는 recv send만 IOCP 모델에 맞는 함수
