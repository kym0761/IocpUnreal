#include "pch.h"
#include "NetAddress.h"

FNetAddress::FNetAddress(SOCKADDR_IN sockAddr) : SockAddr(sockAddr)
{
}

FNetAddress::FNetAddress(wstring ip, uint16 port)
{
	//tcp 사용 여부, ip , port 설정

	::memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr = Ip2Address(ip.c_str());
	SockAddr.sin_port = ::htons(port);
}

wstring FNetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &SockAddr.sin_addr, buffer, len32(buffer));
	return wstring(buffer);
}

IN_ADDR FNetAddress::Ip2Address(const WCHAR* ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
