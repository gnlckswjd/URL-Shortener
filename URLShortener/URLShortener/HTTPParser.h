#pragma once

class HTTPParser
{
public:
	static bool IsValid(class Session* session);
	static bool ParsePacket(class Session* session, string& token);
	
};

