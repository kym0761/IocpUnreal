#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

FIocpCore::FIocpCore()
{
	IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(IocpHandle != INVALID_HANDLE_VALUE);
}

FIocpCore::~FIocpCore()
{
	::CloseHandle(IocpHandle);
}

bool FIocpCore::Register(IocpObjectRef iocpObject)
{
	return ::CreateIoCompletionPort(
		iocpObject->GetHandle(), 
		IocpHandle, 
		0, //key
		0);
}

bool FIocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	FIocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(
		IocpHandle,
		OUT &numOfBytes,
		OUT &key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent),
		timeoutMs //시간 제한
	))
	{
		//처리가 가능하다면 iocp event의 iocp object 가져와서 dispatch 처리
		IocpObjectRef iocpObject = iocpEvent->GetOwner();
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT: //시간 제한 걸릴시 false로 빠져나옴.
			return false;
		default:
			
			//로그 찍기가 필요할지도?

			IocpObjectRef iocpObject = iocpEvent->GetOwner();
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
