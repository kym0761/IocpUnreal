#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class FIocpEvent;
class FService;

/*--------------
	Session
---------------*/

class FSession : public FIocpObject
{
	friend class FListener;
	friend class FIocpCore;
	friend class FService;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

public:
	FSession();
	virtual ~FSession();

public:

	/* 외부에서 사용 */
	void				Send(SendBufferRef sendBuffer);
	bool				Connect();
	void				Disconnect(const WCHAR* cause);

	shared_ptr<FService> GetService() { return Service.lock(); }
	void				SetService(shared_ptr<FService> service) { Service = service; }

public:
	/* 정보 관련 */
	void SetNetAddress(FNetAddress address) { NetAddress = address; }
	FNetAddress	GetAddress() { return NetAddress; }
	SOCKET GetSocket() { return Socket; }
	bool IsConnected() { return bConnected; }
	SessionRef GetSessionRef() { return static_pointer_cast<FSession>(shared_from_this()); }


private:
	/* 인터페이스 구현 */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(FIocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* 전송 관련 */
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(int32 numOfBytes);
	void				ProcessSend(int32 numOfBytes);

	void				HandleError(int32 errorCode);

protected:
	/* 컨텐츠 코드에서 오버로딩해서 쓰게 될 함수 */
	virtual void		OnConnected() { }
	virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }

private:

	// 서비스는 서버가 켜져있는 이상 어딘가에 존재하게 될 것이므로 
	// 굳이 shared_ptr로 레퍼런스 카운팅은 하지 않음.
	weak_ptr<FService>	Service;

	SOCKET			Socket = INVALID_SOCKET; //클라이언트 소켓
	FNetAddress		NetAddress = {};
	Atomic<bool>	bConnected = false;

private:

	/* 수신 관련 */
	FRecvBuffer			RecvBuffer;

	/* 송신 관련 */
	USE_LOCK; // Send & ProcessSend에서 사용함
	queue<SendBufferRef>	SendQueue;
	Atomic<bool>			bSendRegistered = false;

private:
	/* IocpEvent 재사용 */
	FConnectEvent		ConnectEvent;
	FDisconnectEvent	DisconnectEvent;
	FRecvEvent			RecvEvent;
	FSendEvent			SendEvent;
};

/*-----------------
	PacketSession
------------------*/

//패킷 헤더
//데이터를 읽기 위해서 최소한 이 헤더의 크기만큼의 데이터는 받아야함.
//OnRecv 참고
struct FPacketHeader
{
	uint16 size;
	uint16 id; // 프로토콜ID (ex. 1=로그인, 2=이동요청...)
};

class FPacketSession : public FSession
{
public:
	FPacketSession();
	virtual ~FPacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<FPacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed; //사용자가 더이상 override하지 않음.
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract; //사용자는 여기 부분을 건드려야함.
};