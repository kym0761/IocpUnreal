#pragma once

#include "Job.h"
#include "JobQueue.h"

class FRoom : public enable_shared_from_this<FRoom>
{

public:
	FRoom();
	virtual ~FRoom();

	bool HandleEnterPlayerLocked(PlayerRef player);
	bool HandleLeavePlayerLocked(PlayerRef player);

private:
	bool EnterPlayer(PlayerRef player);
	bool LeavePlayer(uint64 objectId);

	USE_LOCK;

private:
	//exceptId = 제외해야할 id
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, PlayerRef> PlayersInRoom;

};

extern RoomRef GRoom;
