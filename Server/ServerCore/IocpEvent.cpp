#include "pch.h"
#include "IocpEvent.h"

FIocpEvent::FIocpEvent(EEventType type) : EventType(type)
{
	Init();
}

void FIocpEvent::Init()
{
	///*OVERLAPPED 변수 초기화 용도*/

	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
