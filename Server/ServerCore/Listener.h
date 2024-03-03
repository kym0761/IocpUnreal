#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class FAcceptEvent; 
class FIocpEvent;

class FListener : public FIocpObject
{
public:
	FListener() = default;
	~FListener();

public:
	/* 외부에서 사용 */
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(FIocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* 수신 관련 */
	void RegisterAccept(FAcceptEvent* acceptEvent);
	void ProcessAccept(FAcceptEvent* acceptEvent);

protected:
	SOCKET Socket = INVALID_SOCKET;
	vector<FAcceptEvent*> AcceptEvents;
	ServerServiceRef Service;
};