// GameServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"

#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

#include "Job.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"

#include "GenProcedures.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;//각 쓰레드마다의 시간제한

		// 네트워크 입출력 처리 -> 패킷 핸들러에 의해 인게임 로직도 처리
		//10ms로 시간제한을 걸어서 빠져나올 수도 있음.
		service->GetIocpCore()->Dispatch(10);

		//예약된 Job 분배
		FThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		//Job을 실행하기 위한 JobQueue를 불러와 Job Execute함
		FThreadManager::DoGlobalQueueWork();
	}
}


int main()
{
	cout << "Server Start\n";

	FServerPacketHandler::Init();

	ServerServiceRef service = make_shared<FServerService>(
		FNetAddress(L"127.0.0.1", 7777),
		make_shared<FIocpCore>(),
		[=]() { return make_shared<FGameSession>(); }, // TODO : SessionManager 등이 필요할지도? -> 람다로 대체
		100);


	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					DoWorkerJob(service);
				}
			});
	}

	////Main Thread도 따로 실행
	//DoWorkerJob(service);

	//room의 updatetick 최초 실행
	GRoom->DoAsync(&FRoom::UpdateTick);

	while (true)
	{
		this_thread::sleep_for(1s);
	}


	GThreadManager->Join();

	return 0;
}