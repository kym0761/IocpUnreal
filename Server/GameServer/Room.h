#pragma once

#include "Job.h"
#include "JobQueue.h"

class FRoom : public FJobQueue
{

public:
	FRoom();
	virtual ~FRoom();

public:
	bool EnterRoom(ObjectRef object, bool randPos = true);
	bool LeaveRoom(ObjectRef object);

public:
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::C2S_MOVE pkt);

	void HandleChatFromPlayer(PlayerRef player, Protocol::C2S_CHAT pkt);

	void HandleJump(uint64 ObjectId);

public:
	void UpdateTick();

	RoomRef GetRoomRef();

	//USE_LOCK; //1안 : room에서 패킷을 처리할때마다 lock을 걸어야함.
	//원래는 Lock을 사용했지만 JobQueue방식으로 변경해서 JobQueue안의 LockQueue가 Lock 동작함.
	//2안 : room에서 lock을 잡지 않는다. JobQueue로 해결한다.
	
private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

private:
	//exceptId = 제외해야할 id
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:

	//Objects in the room
	unordered_map<uint64, ObjectRef> Objects;

public:

	string ServerStartTime;
};

extern RoomRef GRoom;
