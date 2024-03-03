#pragma once

class FIocpEvent;

// 소켓, 세션 리스너 등의 역할을 할 오브젝트
class FIocpObject : public enable_shared_from_this<FIocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(FIocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class FIocpCore
{
public:
	FIocpCore();
	~FIocpCore();

	HANDLE		GetHandle() { return IocpHandle; }

	bool		Register(IocpObjectRef iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE		IocpHandle;
};
