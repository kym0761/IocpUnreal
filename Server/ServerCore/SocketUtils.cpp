#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX		FSocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	FSocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		FSocketUtils::AcceptEx = nullptr;

void FSocketUtils::Init()
{
	//windows 소켓 버전 2.2 사용한다는 세팅
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT &wsaData) == 0);

	/* 런타임에 주소 얻어오는 API */
	SOCKET dummySocket = CreateSocket();

	ASSERT_CRASH(BindWindowsFunction(dummySocket,
		WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket,
		WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket,
		WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
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
	return SOCKET_ERROR != ::WSAIoctl(
		socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid,  //extension function guid
		sizeof(guid),
		fn, //extension function pointer
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
		WSA_FLAG_OVERLAPPED //Iocp이므로 Overlapped 사용.
	); 
}

bool FSocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	//소켓을 Close할 때 전송되지 않은 데이터 처리를 어떻게 할 것인지?

	//비활성 상태 : 소켓 버퍼에 남은 데이터를 전부 전송하는 일반적인 소켓 정상 종료
	// 
	//활성 & l_linger == 0 : close로 즉시 상태 종료 
	// & 소켓 버퍼의 데이터 전부 버림 
	// & tcp 연결 상태일 땐 호스트에 리셋을 위한 RST 패킷을 보냄
	// 
	//활성 & l_linger != 0 : 지정된 시간동안 대기하고 버퍼에 남은 데이터를 모두 보내기를 시도.
	// & 지정된 시간 내에 데이터를 전부 보내면 정상 리턴,
	// & 시간이 초과되면 에러 및 비정상 종료

	LINGER option;
	option.l_onoff = onoff;//linger on off 여부
	option.l_linger = linger;//Linger 옵션이 활성화 되었을 때 기다리는 시간

	return ::SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool FSocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	//주소 재사용을 의미함.
	//커널에서 주소를 완전 종료하지 않고 시간을 두고 계속 점유하는 부분을 해결
	return ::SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool FSocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	//커널 recv 버퍼 크기
	return ::SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool FSocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	//커널 send 버퍼크기
	return ::SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool FSocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	//nagle 알고리즘 사용할지 안할지 여부 체크
	//사용 안함!
	return ::SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool FSocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	//ListenSocket의 특성을 ClientSocket에 그대로 적용함.
	return ::SetSockOpt(socket,
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT, 
		listenSocket);
}

bool FSocketUtils::Bind(SOCKET socket, FNetAddress netAddr)
{
	//FListener는 미리 정의된 주소를 사용해 bind함.

	return SOCKET_ERROR != ::bind(
		socket,
		reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()),
		sizeof(SOCKADDR_IN));
}

bool FSocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY); //아무 주소나 임의로 알아서 연결하라는 의미
	myAddress.sin_port = ::htons(port); //port도 값이 0 이면 임의로 남는 포트를 찾아서 들어감

	return SOCKET_ERROR != ::bind(
		socket, 
		reinterpret_cast<const SOCKADDR*>(&myAddress),
		sizeof(myAddress));
}

bool FSocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != 
		listen(
			socket, //서버 소켓
			backlog //Listen 동작에 필요한 Queue 사이즈
		);
}

void FSocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
	{
		::closesocket(socket);
	}
	socket = INVALID_SOCKET;
}
