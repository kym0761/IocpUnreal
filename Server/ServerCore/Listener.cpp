#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

FListener::~FListener()
{
	FSocketUtils::Close(Socket);

	for (FAcceptEvent* acceptEvent : AcceptEvents)
	{
		// TODO

		//xdelete(acceptEvent);
		delete(acceptEvent);
	}
}

bool FListener::StartAccept(ServerServiceRef service)
{
	Service = service;
	if (Service == nullptr)
	{
		return false;
	}

	Socket = FSocketUtils::CreateSocket();
	if (Socket == INVALID_SOCKET)
	{
		return false;
	}

	//accept할 이 Listener를 IOCP에 등록한다.
	if (Service->GetIocpCore()->Register(shared_from_this()) == false)
	{
		return false;
	}

	//주소 재사용 여부
	if (FSocketUtils::SetReuseAddress(Socket, true) == false)
	{
		return false;
	}

	//옵션
	if (FSocketUtils::SetLinger(Socket, 0, 0) == false)
	{
		return false;
	}

	//Bind
	if (FSocketUtils::Bind(Socket, Service->GetNetAddress()) == false)
	{
		return false;
	}

	//Listen
	if (FSocketUtils::Listen(Socket) == false)
	{
		return false;
	}

	//Main GameServer에서 서비스 생성시 MaxSessionCount를 명시함
	//accept 최대 갯수만큼 acceptEvent 등록
	const int32 acceptCount = Service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		FAcceptEvent* acceptEvent = new FAcceptEvent();
		acceptEvent->SetOwner(shared_from_this()); //acceptEvent의 Owner 세팅
		AcceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void FListener::CloseSocket()
{
	FSocketUtils::Close(Socket);
}

HANDLE FListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket);
}

void FListener::Dispatch(FIocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->GetEventType() == EEventType::Accept);
	FAcceptEvent* acceptEvent = static_cast<FAcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void FListener::RegisterAccept(FAcceptEvent* acceptEvent)
{
	SessionRef session = Service->CreateSession(); // Register IOCP

	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD bytesReceived = 0;
	if (false == FSocketUtils::AcceptEx(
		Socket,
		session->GetSocket(),
		session->RecvBuffer.GetWritePos(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT & bytesReceived,
		static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			//어떠한 이유로 Accept가 실패했다.
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}

void FListener::ProcessAccept(FAcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->GetSession();

	if (false == FSocketUtils::SetUpdateAcceptSocket(session->GetSocket(), Socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddress = sizeof(sockAddress);
	
	if (SOCKET_ERROR == ::getpeername(
		session->GetSocket(),
		OUT reinterpret_cast<SOCKADDR*>(&sockAddress),
		&sizeOfSockAddress))
	{

		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(FNetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);

}
