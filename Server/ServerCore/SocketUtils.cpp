#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX		FSocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	FSocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		FSocketUtils::AcceptEx = nullptr;

void FSocketUtils::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT &wsaData) == 0);

	/* 런타임에 주소 얻어오는 API */
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	Close(dummySocket);

}

void FSocketUtils::Clear()
{
	::WSACleanup();
}

bool FSocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;

	//WSAIoctl은 소켓 모드를 제어하는 함수다.
	return SOCKET_ERROR != WSAIoctl(
		socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid,
		sizeof(guid),
		fn,
		sizeof(*fn),
		OUT & bytes,
		NULL,
		NULL);
}

SOCKET FSocketUtils::CreateSocket()
{
	return WSASocket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP, 
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);
}

bool FSocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool FSocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool FSocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool FSocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool FSocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool FSocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	//ListenSocket의 특성을 ClientSocket에 그대로 적용함.
	return SetSockOpt(socket,
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT, 
		listenSocket);
}

bool FSocketUtils::Bind(SOCKET socket, FNetAddress netAddr)
{
	return SOCKET_ERROR != ::bind(
		socket,
		reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()),
		sizeof(SOCKADDR_IN));
}

bool FSocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(
		socket, 
		reinterpret_cast<const SOCKADDR*>(&myAddress),
		sizeof(myAddress));
}

bool FSocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

void FSocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}
