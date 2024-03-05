#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

FSession::FSession() : RecvBuffer(BUFFER_SIZE)
{
	Socket = FSocketUtils::CreateSocket();
}

FSession::~FSession()
{
	FSocketUtils::Close(Socket);
}

void FSession::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	bool bRegisterSend = false;

	{
		// 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
		WRITE_LOCK;

		//아직 bSendRegistered가 통과되지 않았다면 sendbuffer에 있는 데이터를 sendqueue에 넣어 한번에 보낼 준비를 한다.
		SendQueue.push(sendBuffer);

		/* 설명
		if (_sendRegistered == false)
		{
			_sendRegistered = true;
			RegisterSend();
		}*/

		if (bSendRegistered.exchange(true) == false)
		{
			bRegisterSend = true;
		}

		if (bRegisterSend)
		{
			RegisterSend();
		}
	}
}

bool FSession::Connect()
{
	return RegisterConnect();
}

void FSession::Disconnect(const WCHAR* cause)
{
	if (bConnected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << cause << endl;

	RegisterDisconnect();
}

HANDLE FSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(Socket);
}

void FSession::Dispatch(FIocpEvent* iocpEvent, int32 numOfBytes)
{
	//IOCP에서 복원된 iocpEvent의 eventType에 맞춰 dispatch를 진행함.

	switch (iocpEvent->GetEventType())
	{
	case EEventType::Connect:
		ProcessConnect();
		break;
	case EEventType::Disconnect:
		ProcessDisconnect();
		break;
	case EEventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EEventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool FSession::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != EServiceType::Client)
		return false;

	//주소 재활용
	if (FSocketUtils::SetReuseAddress(Socket, true) == false)
		return false;

	//남는 주소 및 port를 사용해서 bind. 0 == 남은 것 사용
	if (FSocketUtils::BindAnyAddress(Socket, 0) == false)
		return false;

	ConnectEvent.Init();
	ConnectEvent.SetOwner(shared_from_this()); // ADD_REF

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == FSocketUtils::ConnectEx(
		Socket, 
		reinterpret_cast<SOCKADDR*>(&sockAddr), 
		sizeof(sockAddr),
		nullptr,
		0,
		&numOfBytes,
		&ConnectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			ConnectEvent.SetOwner(nullptr); // RELEASE_REF
			return false;
		}
	}

	return true;
}

bool FSession::RegisterDisconnect()
{
	DisconnectEvent.Init();
	DisconnectEvent.SetOwner(shared_from_this()); // ADD_REF

	//소켓 재활용
	if (false == FSocketUtils::DisconnectEx(
		Socket, &DisconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			DisconnectEvent.SetOwner(nullptr); // RELEASE_REF
			return false;
		}
	}

	return true;
}

void FSession::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	RecvEvent.Init();
	RecvEvent.SetOwner(shared_from_this()); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(RecvBuffer.GetWritePos());
	wsaBuf.len = RecvBuffer.GetFreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(
		Socket,
		&wsaBuf,
		1,
		OUT & numOfBytes,
		OUT & flags,
		&RecvEvent,
		nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			RecvEvent.SetOwner(nullptr); // 오류로 RELEASE_REF
		}
	}
}

void FSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	SendEvent.Init();
	SendEvent.SetOwner(shared_from_this()); // ADD_REF

	// 보낼 데이터를 SendEvent에 등록
	{
		//WRITE_LOCK;

		int32 writeSize = 0;
		while (SendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = SendQueue.front();

			writeSize += sendBuffer->GetWriteSize();
			// TODO : 예외 체크

			SendQueue.pop();
			SendEvent.SendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한 방에 보낸다)
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(SendEvent.SendBuffers.size());
	for (SendBufferRef sendBuffer : SendEvent.SendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->GetBuffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->GetWriteSize());
		wsaBufs.push_back(wsaBuf);
	}


	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(
		Socket,
		wsaBufs.data(), //데이터의 위치
		static_cast<DWORD>(wsaBufs.size()), //보낼 데이터의 크기
		OUT &numOfBytes,
		0,
		&SendEvent,
		nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)//펜딩을 제외한 실패는 레퍼런스 카운트로 살려주고 있던 오브젝트들을 전부 풀어줘야함.
		{
			HandleError(errorCode);
			SendEvent.SetOwner(nullptr); // RELEASE_REF
			SendEvent.SendBuffers.clear(); // RELEASE_REF
			bSendRegistered.store(false);
		}
	}
}

void FSession::ProcessConnect()
{
	ConnectEvent.SetOwner(nullptr);// RELEASE_REF

	bConnected.store(true);

	//서비스에 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 이 기능은 컨텐츠 단에서 오버라이드된 Onconnected가 동작할 것임
	OnConnected();

	// 처음으로 수신 등록됨
	RegisterRecv();
}

void FSession::ProcessDisconnect()
{
	DisconnectEvent.SetOwner(nullptr); // RELEASE_REF

	OnDisconnected(); // 컨텐츠 코드에서 오버라이딩
	GetService()->ReleaseSession(GetSessionRef());
}

void FSession::ProcessRecv(int32 numOfBytes)
{
	RecvEvent.SetOwner(nullptr); // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (RecvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = RecvBuffer.GetDataSize();
	int32 processLen = OnRecv(RecvBuffer.GetReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || RecvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	// 커서 정리
	RecvBuffer.Clean();

	// 수신 등록
	RegisterRecv();
}

void FSession::ProcessSend( int32 numOfBytes)
{
	SendEvent.SetOwner(nullptr); // RELEASE_REF
	SendEvent.SendBuffers.clear(); // RELEASE_REF


	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 오버로딩
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (SendQueue.empty()) // sendqueue를 다 보내줬으면 bSendRegistered false하여 다음 send가 동작하도록 함
	{
		bSendRegistered.store(false);
	}
	else
	{
		RegisterSend(); // 아직 남아있다면 register send를 다시 진행
	}

}

void FSession::HandleError(int32 errorCode)
{
	//연결 실패의 경우의 수
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}

//~~

FPacketSession::FPacketSession()
{
}

FPacketSession::~FPacketSession()
{
}

//데이터 조립
// ex: [size(2)][id(2)][data....] [size(2)][id(2)][data....]
int32 FPacketSession::OnRecv(BYTE* buffer, int32 len)
{
	//받은 패킷 얼마만큼 처리했는지 확인
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;
		// 최소한 헤더는 파싱할 수 있어야 한다
		if (dataSize < sizeof(FPacketHeader))
			break;

		FPacketHeader header 
			= *(reinterpret_cast<FPacketHeader*>(
				&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
