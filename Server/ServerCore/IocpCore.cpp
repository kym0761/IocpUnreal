#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

FIocpCore::FIocpCore()
{
	//Iocp 핸들 생성
	IocpHandle = ::CreateIoCompletionPort(
		INVALID_HANDLE_VALUE, 
		0,
		0,
		0 // 쓰레드 수. 0 == 시스템에서 가능한 코어 수
	);

	ASSERT_CRASH(IocpHandle != INVALID_HANDLE_VALUE);
}

FIocpCore::~FIocpCore()
{
	::CloseHandle(IocpHandle);
}

bool FIocpCore::Register(IocpObjectRef iocpObject)
{
	//FListener라면 서버 소켓을 Iocp에 등록함.
	//FSession이면 세션의 소켓(클라이언트 소켓)을 Iocp에 등록한다.

	//iocpEvent(OVERLAPPED)의 Owner로 IocpObject를 뽑을 것이므로 키를 굳이 특정 값으로 세팅하지 않음.
	//아래 FIocpCore::Dispatch()를 확인할 것

	return ::CreateIoCompletionPort(
		iocpObject->GetHandle(), //등록할 소켓
		IocpHandle, //존재하는 Iocp 핸들
		0, //key
		0 // 등록에선 무시
	);
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
		//IO가 완료된 iocp event의 iocp object 가져와서 dispatch 처리
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
