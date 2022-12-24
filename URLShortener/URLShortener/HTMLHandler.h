#pragma once
class HTMLHandler
{
public:

	static bool HandlePacket(class Session* session, string token);
	
	static bool SendMainMsg(Session* session);
	static bool SendShorteningResultMsg(Session* session, const string& url);
	static bool SendRedirectionMsg(Session* session, const string& url);
	static bool Send(Session* session, const char* msg);
};

