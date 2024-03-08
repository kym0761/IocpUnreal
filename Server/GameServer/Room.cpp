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

bool FRoom::EnterRoom(ObjectRef object, bool randPos)
{
	bool success = AddObject(object);

	if (randPos)
	{
		// 일단 임의의 위치에 배치할 계획
		object->PosInfo->set_x(FServerUtils::GetRandom(0.f, 500.f));
		object->PosInfo->set_y(FServerUtils::GetRandom(0.f, 500.f));
		object->PosInfo->set_z(100.0f);
		object->PosInfo->set_yaw(FServerUtils::GetRandom(0.f, 100.f));
	}

	// 입장 사실을 지금 들어온 플레이어에게 알림
	if (auto player = dynamic_pointer_cast<FPlayer>(object))
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::ObjectInfo* objectInfo = new Protocol::ObjectInfo();
		objectInfo->CopyFrom(*object->ObjectInfo);
		enterGamePkt.set_allocated_player(objectInfo);
		//enterGamePkt.release_player(); //set allocated __ 는 자동으로 풀어주니 상관없음.

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->Session.lock())
			session->Send(sendBuffer);
	}

	// 입장 사실을 이미 room에 있던 다른 플레이어들에게 알린다
	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
		objectInfo->CopyFrom(*object->ObjectInfo);

		SendBufferRef sendBuffer =
			FServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, object->ObjectInfo->object_id());

	}


	// 기존에 입장한 플레이어 목록을 새로 들어온 플레이어에게 전송
	if (auto player = dynamic_pointer_cast<FPlayer>(object))
	{
		Protocol::S_SPAWN spawnPkt;

		for (auto& item : Objects)
		{
			if (item.second->IsPlayer() == false)
			{
				continue;
			}

			Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(*item.second->ObjectInfo);
		}

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto session = player->Session.lock())
			session->Send(sendBuffer);
	}

	return success;
}

bool FRoom::LeaveRoom(ObjectRef object)
{
	if (object == nullptr)
		return false;

	//WRITE_LOCK;

	const uint64 objectId = object->ObjectInfo->object_id();
	bool success = RemoveObject(objectId); //메모리 상에서는 여기서 사라짐.

	// 퇴장 사실을 퇴장하는 플레이어에게 알린다
	if (auto player = dynamic_pointer_cast<FPlayer>(object))
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = 
			FServerPacketHandler::MakeSendBuffer(leaveGamePkt);
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

		if (auto player = dynamic_pointer_cast<FPlayer>(object))
		{
			if (auto session = player->Session.lock())
			{
				session->Send(sendBuffer);
			}
		}
		
	}

	cout << "player " << object->ObjectInfo->object_id() << " is leaved.." << endl;

	return success;
}

bool FRoom::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player, true);
}

bool FRoom::HandleLeavePlayer(PlayerRef player)
{
	return LeaveRoom(player);
}

void FRoom::HandleMove(Protocol::C_MOVE pkt)
{
	//WRITE_LOCK;

	//없는 플레이어를 이동시키려고 함
	const uint64 objectId = pkt.info().object_id();
	if (Objects.find(objectId) == Objects.end())
	{
		return;
	}

	//!! 있는 플레이어지만, 비정상적인 이동을 하려고 한다
	//!! 아마도 해커거나 핵이나 아무튼 별로 좋은 일은 아니다.
	//!! 그 부분에 대한 대비를 해야하지만 여기서는 스킵한다.

	//이동한 플레이어의 정보 갱신
	PlayerRef player = dynamic_pointer_cast<FPlayer>(Objects[objectId]);
	player->PosInfo->CopyFrom(pkt.info());

	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}

		//직렬화 후 broadcast.
		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer);
	}

}

void FRoom::HandleChat(Protocol::C_CHAT pkt)
{
}

void FRoom::HandleChatFromPlayer(PlayerRef player, Protocol::C_CHAT pkt)
{

	string str = pkt.msg();

	uint64 id = player->ObjectInfo->object_id();

	Protocol::S_CHAT sendChatPkt;

	{
		string* ms = sendChatPkt.mutable_msg();
		*ms = to_string(id) + " Player : " + str;
		sendChatPkt.set_playerid(id);
	}

	SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(sendChatPkt);
	Broadcast(sendBuffer);
}

void FRoom::UpdateTick()
{
	//cout << "Update Room" << endl;

	//100ms마다 (0.1초) updatetick을 재실행
	DoTimer(100, &FRoom::UpdateTick);
}

RoomRef FRoom::GetRoomRef()
{
	return  static_pointer_cast<FRoom>(shared_from_this());
}

//bool FRoom::EnterPlayer(PlayerRef player)
//{
//	////handleEnter에서 lock을 걸었으므로, 여기서 걸면 안됨!
//
//	//// 있다면 문제가 있다.
//	//if (PlayersInRoom.find(player->ObjectInfo->object_id()) != PlayersInRoom.end())
//	//	return false;
//
//	//PlayersInRoom.insert(
//	//	make_pair(player->ObjectInfo->object_id(), player));
//
//	//player->Room.store(GetRoomRef());
//
//	//return true;
//}
//
//bool FRoom::LeavePlayer(uint64 objectId)
//{
//	//// 없다면 문제가 있다.
//	//if (PlayersInRoom.find(objectId) == PlayersInRoom.end())
//	//	return false;
//
//	//PlayerRef player = PlayersInRoom[objectId];
//	//player->Room.store(weak_ptr<FRoom>());
//
//	//PlayersInRoom.erase(objectId);
//
//	//return true;
//}

bool FRoom::AddObject(ObjectRef object)
{
	//handleEnter에서 lock을 걸었으므로, 여기서 걸면 안됨!

	// 있다면 문제가 있다.
	if (Objects.find(object->ObjectInfo->object_id()) != Objects.end())
		return false;

	Objects.insert(
		make_pair(object->ObjectInfo->object_id(), object));

	object->Room.store(GetRoomRef());

	return true;
}

bool FRoom::RemoveObject(uint64 objectId)
{
	// 없다면 문제가 있다.
	if (Objects.find(objectId) == Objects.end())
		return false;

	ObjectRef object = Objects[objectId];
	PlayerRef player = dynamic_pointer_cast<FPlayer>(object);
	if (player)
	{
		player->Room.store(weak_ptr<FRoom>());
	}
	
	Objects.erase(objectId);

	return true;
}

void FRoom::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : Objects)
	{
		PlayerRef player = dynamic_pointer_cast<FPlayer>(item.second);

		if (player == nullptr)
		{
			continue;
		}

		if (player->ObjectInfo->object_id() == exceptId) // 해당 id는 제외
		{
			continue;
		}

		if (GameSessionRef session = player->Session.lock())
		{
			session->Send(sendBuffer);
		}
	}
}
