#pragma once

//주소, 포트 지정

class FNetAddress
{
public:
	FNetAddress() = default;
	FNetAddress(SOCKADDR_IN sockAddr);
	FNetAddress(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return SockAddr; }
	wstring			GetIpAddress();
	uint16			GetPort() { return ::ntohs(SockAddr.sin_port); }

public:
	static IN_ADDR	Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN		SockAddr = {};
};

