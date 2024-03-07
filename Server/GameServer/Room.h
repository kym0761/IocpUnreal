#pragma once

#include "Job.h"
#include "JobQueue.h"

class FRoom : public FJobQueue
{

public:
	FRoom();
	virtual ~FRoom();

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::C_MOVE pkt);

	RoomRef GetRoomRef();

private:
	bool EnterPlayer(PlayerRef player);
	bool LeavePlayer(uint64 objectId);


	//2안 : room에서 lock을 잡지 않는다.
	//USE_LOCK; //1안 : room에서 패킷을 처리할때마다 lock을 걸어야함.

private:
	//exceptId = 제외해야할 id
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, PlayerRef> PlayersInRoom;

};

extern RoomRef GRoom;
