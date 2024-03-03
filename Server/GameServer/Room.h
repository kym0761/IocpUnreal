#pragma once

#include "Job.h"
#include "JobQueue.h"

//일단 방이 한개 있다고 가정함.
//Room 자체가 JobQueue가 되어 Room에서 동작해야할 Job을 처리한다.
class FRoom :public FJobQueue
{

//public:
//
//	// 싱글쓰레드 환경인마냥 코딩
//	void Enter(PlayerRef player);
//	void Leave(PlayerRef player);
//	void Broadcast(SendBufferRef sendBuffer); //방에 있는 플레이어에게 broadcast
//
//private:
//	map<uint64, PlayerRef> Players;

};

extern shared_ptr<FRoom> GRoom;
