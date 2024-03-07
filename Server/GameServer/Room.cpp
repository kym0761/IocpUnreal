#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

RoomRef GRoom = make_shared<FRoom>();

FRoom::FRoom()
{
}

FRoom::~FRoom()
{
}

bool FRoom::HandleEnterPlayer(PlayerRef player)
{
	//WRITE_LOCK;

	bool success = EnterPlayer(player);

	// 일단 임의의 위치에 배치할 계획
	player->PlayerInfo->set_x(FServerUtils::GetRandom(0.f, 500.f));
	player->PlayerInfo->set_y(FServerUtils::GetRandom(0.f, 500.f));
	player->PlayerInfo->set_z(100.0f);
	player->PlayerInfo->set_yaw(FServerUtils::GetRandom(0.f, 100.f));


	// 입장 사실을 지금 들어온 플레이어에게 알림
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(*player->PlayerInfo);
		enterGamePkt.set_allocated_player(playerInfo);
		//enterGamePkt.release_player(); //set allocated __ 는 자동으로 풀어주니 상관없음.

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->Session.lock())
			session->Send(sendBuffer);
	}

	// 입장 사실을 이미 room에 있던 다른 플레이어들에게 알린다
	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
		playerInfo->CopyFrom(*player->PlayerInfo);

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, player->PlayerInfo->object_id());

	}


	// 기존에 입장한 플레이어 목록을 새로 들어온 플레이어에게 전송
	{
		Protocol::S_SPAWN spawnPkt;

		for (auto& item : PlayersInRoom)
		{
			Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(*item.second->PlayerInfo);
		}

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto session = player->Session.lock())
			session->Send(sendBuffer);
	}

	return true;
}

bool FRoom::HandleLeavePlayer(PlayerRef player)
{
	if (player == nullptr)
		return false;

	//WRITE_LOCK;

	const uint64 objectId = player->PlayerInfo->object_id();
	bool success = LeavePlayer(objectId); //메모리 상에서는 여기서 사라짐.

	// 퇴장 사실을 퇴장하는 플레이어에게 알린다
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->Session.lock())
			session->Send(sendBuffer);
	}

	// 퇴장 사실을 알린다
	//despawn한다.
	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectId);

		if (auto session = player->Session.lock())
			session->Send(sendBuffer);
	}

	cout << "player " << player->PlayerInfo->object_id() << " is leaved.." << endl;

	return success;
}

void FRoom::HandleMove(Protocol::C_MOVE pkt)
{
	//WRITE_LOCK;

	//없는 플레이어를 이동시키려고 함
	const uint64 objectId = pkt.info().object_id();
	if (PlayersInRoom.find(objectId) == PlayersInRoom.end())
	{
		return;
	}

	//!! 있는 플레이어지만, 비정상적인 이동을 하려고 한다
	//!! 아마도 해커거나 핵이나 아무튼 별로 좋은 일은 아니다.
	//!! 그 부분에 대한 대비를 해야하지만 여기서는 스킵한다.

	//이동한 플레이어의 정보 갱신
	PlayerRef& player = PlayersInRoom[objectId];
	player->PlayerInfo->CopyFrom(pkt.info());
		
	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PlayerInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}

		//직렬화 후 broadcast.
		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer);

	}

}

RoomRef FRoom::GetRoomRef()
{
	return  static_pointer_cast<FRoom>(shared_from_this());
}

bool FRoom::EnterPlayer(PlayerRef player)
{
	//handleEnter에서 lock을 걸었으므로, 여기서 걸면 안됨!

	// 있다면 문제가 있다.
	if (PlayersInRoom.find(player->PlayerInfo->object_id()) != PlayersInRoom.end())
		return false;

	PlayersInRoom.insert(
		make_pair(player->PlayerInfo->object_id(), player));

	player->Room.store(GetRoomRef());

	return true;
}

bool FRoom::LeavePlayer(uint64 objectId)
{
	// 없다면 문제가 있다.
	if (PlayersInRoom.find(objectId) == PlayersInRoom.end())
		return false;

	PlayerRef player = PlayersInRoom[objectId];
	player->Room.store(weak_ptr<FRoom>());

	PlayersInRoom.erase(objectId);

	return true;
}

void FRoom::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : PlayersInRoom)
	{
		PlayerRef player = item.second;
		if (player->PlayerInfo->object_id() == exceptId) // 해당 id는 제외
		{
			continue;
		}

		if (GameSessionRef session = player->Session.lock())
		{
			session->Send(sendBuffer);
		}
	}
}
