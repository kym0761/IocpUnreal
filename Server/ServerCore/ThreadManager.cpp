#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"


FThreadManager::FThreadManager()
{
	//main thread 1
	InitTLS();

}

FThreadManager::~FThreadManager()
{
	Join();
}

void FThreadManager::Launch(function<void(void)> Callback)
{
	LockGuard lockguard(Lock); //Launch가 끝나면 lock이 자동으로 풀린다.

	Threads.push_back(
		thread([=]() //Callback을 실행해줄 쓰레드 생성
		{
			InitTLS();
			Callback();
			DestroyTLS();
		}));
}

void FThreadManager::Join()
{
	for (thread& t : Threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}


	Threads.clear();
}

void FThreadManager::InitTLS()
{
	static Atomic<uint32> sThreadId = 1;
	
	//TLS : See CoreTLS.h
	LThreadId = sThreadId.fetch_add(1);

}

void FThreadManager::DestroyTLS()
{
	//?
}

void FThreadManager::DoGlobalQueueWork()
{
	//JobQueue를 얻어 일처리를 하려고 시도함.
	
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > LEndTickCount) //thread local에 있는 제한시간을 초과
		{
			break;
		}

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
		{
			break;
		}
			
		jobQueue->Execute();
	}
}

void FThreadManager::DistributeReservedJobs()
{
	const uint64 now = ::GetTickCount64();

	GJobTimer->Distribute(now);
}
