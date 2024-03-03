#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"
void FLock::WriteLock(const char* Name)
{
//#if _DEBUG
//	GDeadLockProfiler->PushLock(Name);
//#endif

	LockFlag;
	//동일한 쓰레드가 소유하고 있다면 무조건 성공함.
	const uint32 lockThreadId = (LockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		WriteCount++;
		return;
	}


	//아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 획득함.
	const int64 beginTick = GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK); //현재 쓰레드 ID를 write flag로 변환
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;

			// empty(expected) 상태면 위의 desired로 write flag를 set해준다.
			if (LockFlag.compare_exchange_strong(OUT expected, desired))
			{
				WriteCount++;
				return;
			}
		}

		//Lock을 얻으려는 시도가 너무 오래걸리면 실패로 취급함.
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK TIMEOUT");
		}

		this_thread::yield();
	}

}

void FLock::WriteUnlock(const char* Name)
{
//#if _DEBUG
//	GDeadLockProfiler->PopLock(Name);
//#endif


	//READ LOCK이 다 풀리기 전에 WRITE Unlock을 하면 안됨.
	//Write -> Write ok
	//write -> read ok
	//read -> write no
	if ((LockFlag.load() & READ_COUNT_MASK) != 0)
	{
		CRASH("INVALID UNLOCK ORDER");
	}


	const int32 lockcount = --WriteCount;
	if (lockcount == 0)
	{
		LockFlag.store(EMPTY_FLAG);
	}

}

void FLock::ReadLock(const char* Name)
{
//#if _DEBUG
//	GDeadLockProfiler->PushLock(Name);
//#endif

	//동일한 쓰레드가 Write 하는 중이라면 무조건 성공함.
	const uint32 lockThreadId = (LockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		LockFlag.fetch_add(1);
		return;
	}

	//아무도 소유하고 있지 않았다면 경합하여 공유 카운트 획득
	const int64 beginTick = GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (LockFlag.load() & READ_COUNT_MASK);

			// 공유카운트를 1 올려준다.
			if (LockFlag.compare_exchange_strong(OUT expected, expected + 1))
			{
				return;
			}
		}

		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK TIMEOUT");
		}

		this_thread::yield();
	}

}

void FLock::ReadUnlock(const char* Name)
{
//#if _DEBUG
//	GDeadLockProfiler->PopLock(Name);
//#endif


	//lockflag가 1을 빼기 전에 이미 0이었다는 의미이므로 실패다.
	if ((LockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE UNLOCK")
	}


}
