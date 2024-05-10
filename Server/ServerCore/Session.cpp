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
	//cout << "~FSession()" << endl;
	FSocketUtils::Close(Socket);
}

void FSession::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
	{
		return;
	}

	//registerSend를 동작하기 위한 bool 변수
	bool bRegisterSend = false;

	{
		// 현재 RegisterSend()가 걸리지 않은 상태라면, 걸어준다
		WRITE_LOCK;

		//아직 bSendRegistered가 통과되지 않았다면 sendbuffer에 있는 데이터를 sendqueue에 넣어 한번에 보낼 준비를 한다.
		SendQueue.push(sendBuffer);

		/* 설명
		if (bSendRegistered == false)
		{
			bSendRegistered = true;
			RegisterSend();
		}*/

		//send 등록이 되지 않았다면, true로 세팅하도록 한다.
		//send 등록이 된 상태면 SendQueue에 sendBuffer만 넣어주고 Send()를 종료함.
		if (bSendRegistered.exchange(true) == false)
		{
			bRegisterSend = true;
		}

		//위에서 bRegisterSend값이 true가 되면 실질적인 Send 동작을 시작함.
		if (bRegisterSend)
		{
			RegisterSend();
		}
	}
}

bool FSession::Connect()
{
	//connection은 클라이언트 소켓의 행동이므로, dummy client가 아니면 실행되지 않을 것임.
	return RegisterConnect();
}

void FSession::Disconnect(const WCHAR* cause)
{
	if (bConnected.exchange(false) == false)
	{
		return;
	}

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
	{
		return false;
	}
		
	if (GetService()->GetServiceType() != EServiceType::Client)
	{
		return false;
	}

	//주소 재활용
	if (FSocketUtils::SetReuseAddress(Socket, true) == false)
	{
		return false;
	}

	//남는 주소 및 port를 사용해서 bind. 0 == 남은 것 사용
	if (FSocketUtils::BindAnyAddress(Socket, 0) == false)
	{
		return false;
	}

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

	//소켓 재활용 옵션 사용
	if (false == FSocketUtils::DisconnectEx(
		Socket, 
		&DisconnectEvent, 
		TF_REUSE_SOCKET,
		0))
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
	{
		return;
	}
		
	RecvEvent.Init();
	RecvEvent.SetOwner(shared_from_this()); // ADD_REF

	//buf 위치부터 len까지 recv에 쓰도록 해라는 의미
	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(RecvBuffer.GetWritePos());
	wsaBuf.len = RecvBuffer.GetFreeSize();

	//Iocp에서는 Recv대신 WSARecv를 사용함.
	//Recv이벤트를 Iocp에 등록
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
	{
		return;
	}

	SendEvent.Init();
	SendEvent.SetOwner(shared_from_this()); // ADD_REF

	// 보낼 데이터를 SendEvent에 등록
	{
		//FSession::Send()에서 Lock을 걸고 시작하므로 여기에선 이제 Lock을 걸지 않음.
		//RegisterSend()가 완료될 때까지 Lock이 걸리므로 queue에 접근하는 다른 쓰레드는 존재하지 않음.
		//WRITE_LOCK;

		int32 writeSize = 0;
		while (SendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = SendQueue.front();

			writeSize += sendBuffer->GetWriteSize();
			// TODO : 예외 체크 필요?

			SendQueue.pop();
			SendEvent.SendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한번에 보냄)
	//보내야하는 데이터를 저장하는 것이 아니라. 보내야할 데이터의 위치를 알려주는 역할이다.
	//WSASend로 넘어가면 커널에서 처리함
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(SendEvent.SendBuffers.size());
	for (SendBufferRef sendBuffer : SendEvent.SendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->GetBuffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->GetWriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	//Iocp에서는 send대신 WSASend를 사용함.
	//Iocp에 Send이벤트를 등록
	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(
		Socket,
		wsaBufs.data(), //데이터의 위치
		static_cast<DWORD>(wsaBufs.size()), //보낼 데이터의 개수
		OUT &numOfBytes,
		0,
		&SendEvent,
		nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)//펜딩을 제외한 실패는 레퍼런스 카운트로 살려주고 있던 오브젝트들을 전부 풀어줘야함.
		{
			HandleError(errorCode);
			SendEvent.SetOwner(nullptr); // IocpObject에 대한 RELEASE_REF
			SendEvent.SendBuffers.clear(); // 보내기 예약을 했던 SendBuffer들에 대한 RELEASE_REF
			bSendRegistered.store(false); //Send()
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
	//데이터는 RegisterRecv에서 Iocp에 WSARecv() 동작이 완료되면 받아짐.

	RecvEvent.SetOwner(nullptr); // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	//recv 버퍼의 크기보다 더 큰 값을 쓰면 실패
	if (RecvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	//processLen이 데이터 범위를 벗어나면 실패
	int32 dataSize = RecvBuffer.GetDataSize();
	int32 processLen = OnRecv(RecvBuffer.GetReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen 
		|| RecvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	// 커서 정리
	//데이터를 정리하는 의미가 아님. 커서를 앞으로 당기려는 행위임
	//RecvBuffer는 처리된 부분까지는 자동으로 위치를 앞으로 당기는 기능이 필요함.
	RecvBuffer.Clean();

	// 수신 재등록
	RegisterRecv();
}

void FSession::ProcessSend(int32 numOfBytes)
{
	SendEvent.SetOwner(nullptr); // IocpObject에 대한 RELEASE_REF
	SendEvent.SendBuffers.clear(); // 보내기 예약을 했던 SendBuffer들에 대한 RELEASE_REF


	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 구현에서 오버라이딩
	OnSend(numOfBytes);

	//SendQueue 경합
	//Send()에서도 Lock을 잡는 것과 비슷한 원리다.
	//이후 만약 RegisterSend()를 할 것이 남아있다면 다시 Lock을 걸어야함.
	WRITE_LOCK;

	// sendqueue를 다 보내줬으면 bSendRegistered false하여 다음 send가 동작하도록 함
	if (SendQueue.empty())
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
	//cout << "~FPacketSession()" << endl;
}

//데이터 조립
// ex: [size(2)][id(2)][data....] [size(2)][id(2)][data....]
int32 FPacketSession::OnRecv(BYTE* buffer, int32 len)
{
	//받은 패킷 얼마만큼 처리했는지 확인
	int32 processLen = 0;

	//처리 불가능한 패킷은 return 값이 0이 나와 RecvBuffer의 사용 처리를 막음
	//recvBuffer의 OnRead가 불려지면서 0바이트 처리했다고 OK가 됨.
	
	//패킷이 여러개 존재한다면 패킷을 동시에 여러개 처리할 수도 있음

	while (true)
	{
		int32 dataSize = len - processLen;
		// 최소한 헤더는 파싱할 수 있어야 한다
		if (dataSize < sizeof(FPacketHeader))
		{
			break;
		}
			

		FPacketHeader header 
			= *(reinterpret_cast<FPacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.size)
		{
			break;
		}

		// 패킷 조립 성공
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
