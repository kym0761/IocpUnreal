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


	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::C_MOVE pkt);

	void HandleChat(Protocol::C_CHAT pkt);
	void HandleChatFromPlayer(PlayerRef player, Protocol::C_CHAT pkt);

public:
	void UpdateTick();

	RoomRef GetRoomRef();

//private:
//	bool EnterPlayer(PlayerRef player);
//	bool LeavePlayer(uint64 objectId);


	//2안 : room에서 lock을 잡지 않는다.
	//USE_LOCK; //1안 : room에서 패킷을 처리할때마다 lock을 걸어야함.

private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

private:
	//exceptId = 제외해야할 id
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:

	//Objects in the room
	unordered_map<uint64, ObjectRef> Objects;
};

extern RoomRef GRoom;
