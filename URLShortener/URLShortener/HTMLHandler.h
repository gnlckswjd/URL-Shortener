#pragma once
class HTMLHandler
{
public:

	static bool HandlePacket(class Session* session, string token);
	
	static bool SendMainMsg(Session* session);
	static bool SendShorteningResultMsg(Session* session, string url);
	static bool SendRedirectionMsg(Session* session, string url);
	

	const char* redirectionMsg = "HTTP/1.0 200 OK\r\n"
		"Content-Length: 200\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<html>\n<head>\n<link rel=\"icon\" href=\"data:, \">\n</head>"
		"\n<body>\nredirection\n</body>\n"
		"</html>";
};

