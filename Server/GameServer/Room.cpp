#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

RoomRef GRoom = make_shared<FRoom>();

FRoom::FRoom()
{
	cout << "Room()" << endl;
}

FRoom::~FRoom()
{
	cout << "~Room()" << endl;
}

bool FRoom::EnterRoom(ObjectRef object, bool randPos)
{
	bool bSuccess = AddObject(object);

	//if (bSuccess == false)
	//{
	//	cout << "AddObject Failed" << endl;
	//	return false;
	//}

	if (randPos)
	{
		// 일단 임의의 위치에 배치할 계획
		object->PosInfo->set_x(FServerUtils::GetRandom(-200.f, 200.f));
		object->PosInfo->set_y(FServerUtils::GetRandom(-200.f, 200.f));
		object->PosInfo->set_z(100.0f);
		object->PosInfo->set_yaw(FServerUtils::GetRandom(0.f, 359.f));
	}

	// 입장 사실을 지금 들어온 플레이어에게 알림
	if (auto player = dynamic_pointer_cast<FPlayer>(object))
	{
		Protocol::S2C_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(bSuccess);

		Protocol::ObjectInfo* objectInfo = new Protocol::ObjectInfo();
		objectInfo->CopyFrom(*object->ObjectInfo);
		enterGamePkt.set_allocated_player(objectInfo);
		//enterGamePkt.release_player(); //set allocated __ 는 자동으로 풀어주니 상관없음.

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->Session.lock())
		{
			session->Send(sendBuffer);
		}
	}

	// 입장 사실을 이미 room에 있던 다른 플레이어들에게 알린다
	{
		Protocol::S2C_SPAWN spawnPkt;

		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
		objectInfo->CopyFrom(*object->ObjectInfo);

		SendBufferRef sendBuffer =
			FServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, object->ObjectInfo->object_id());

	}


	// 기존에 입장한 플레이어 정보들을 새로 들어온 플레이어에게 전송
	if (auto player = dynamic_pointer_cast<FPlayer>(object))
	{
		Protocol::S2C_SPAWN spawnPkt;

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
		{
			session->Send(sendBuffer);
		}
	}

	//TODO : 입장시 채팅창에 알림 
	cout << "Player " << object->ObjectInfo->object_id() << "Entered." << endl;
	{
		Protocol::S2C_CHAT enterChatPkt;

		uint64 id = object->ObjectInfo->object_id();
		string* ms = enterChatPkt.mutable_msg();
		*ms = "Player " + to_string(id) + " Entered.";
		enterChatPkt.set_playerid(id);

		//직렬화 후 broadcast.
		SendBufferRef sendBuffer =
			FServerPacketHandler::MakeSendBuffer(enterChatPkt);
		Broadcast(sendBuffer);
	}


	return bSuccess;
}

bool FRoom::LeaveRoom(ObjectRef object)
{
	if (object == nullptr)
	{
		return false;
	}

	const uint64 objectId = object->ObjectInfo->object_id();
	bool bSuccess = RemoveObject(objectId); //메모리 상에서는 여기서 사라짐.

	//if (bSuccess == false)
	//{
	//	cout << "Remove Object failed." << endl;
	//	return false;
	//}

	// 퇴장하는 플레이어에게 서버의 LeavePacket을 보내 퇴장 상태를 알린다
	if (auto player = dynamic_pointer_cast<FPlayer>(object))
	{
		Protocol::S2C_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = 
			FServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->Session.lock())
		{
			session->Send(sendBuffer);
		}
	}

	// 모든 플레이어에게 퇴장하는 Player의 ObjectId를 보내 해당 캐릭터를 없애도록 함
	//despawn한다.
	{
		Protocol::S2C_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectId); //퇴장하는 ObjectId는 제외하고 Broadcast

		////나가는 당사자인 플레이어도?
		//if (auto player = dynamic_pointer_cast<FPlayer>(object))
		//{
		//	if (auto session = player->Session.lock())
		//	{
		//		session->Send(sendBuffer);
		//	}
		//}
		
	}

	//TODO : 나갈 시 플레이어들에게 알림.
	cout << "player " << object->ObjectInfo->object_id() << " is leaved.." << endl;
	{
		Protocol::S2C_CHAT leaveChatPkt;

		uint64 id = object->ObjectInfo->object_id();
		string* ms = leaveChatPkt.mutable_msg();
		*ms = "Player " + to_string(id) + " is Leaved.";
		leaveChatPkt.set_playerid(id);

		//직렬화 후 broadcast.
		SendBufferRef sendBuffer =
			FServerPacketHandler::MakeSendBuffer(leaveChatPkt);
		Broadcast(sendBuffer);
		
	}


	return bSuccess;
}

bool FRoom::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player, true);
}

bool FRoom::HandleLeavePlayer(PlayerRef player)
{
	return LeaveRoom(player);
}

void FRoom::HandleMove(Protocol::C2S_MOVE pkt)
{
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
		Protocol::S2C_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}

		//직렬화 후 broadcast.
		SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer);
	}

}

void FRoom::HandleChatFromPlayer(PlayerRef player, Protocol::C2S_CHAT pkt)
{
	string str = pkt.msg();
	uint64 id = player->ObjectInfo->object_id();

	Protocol::S2C_CHAT sendChatPkt;

	{
		string* ms = sendChatPkt.mutable_msg();
		*ms = to_string(id) + " Player : " + str;
		sendChatPkt.set_playerid(id);
	}

	SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(sendChatPkt);
	Broadcast(sendBuffer);
}

void FRoom::HandleJump(uint64 ObjectId)
{
	//없는 플레이어를 점프하려고 함
	if (Objects.find(ObjectId) == Objects.end())
	{
		return;
	}

	Protocol::S2C_JUMP jumpPkt;

	{
		jumpPkt.set_playerid(ObjectId);
	}

	SendBufferRef sendBuffer = FServerPacketHandler::MakeSendBuffer(jumpPkt);
	Broadcast(sendBuffer, ObjectId); //플레이어 점프 당사자인 클라이언트는 제외

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

bool FRoom::AddObject(ObjectRef object)
{
	//handleEnter에서 lock을 걸었으므로, 여기서 걸면 안됨!

	// 있다면 문제가 있다.
	if (Objects.find(object->ObjectInfo->object_id()) != Objects.end())
	{
		return false;
	}

	Objects.insert(
		make_pair(object->ObjectInfo->object_id(), object));

	object->Room.store(GetRoomRef());

	return true;
}

bool FRoom::RemoveObject(uint64 objectId)
{
	// 없다면 문제가 있다.
	if (Objects.find(objectId) == Objects.end())
	{
		return false;
	}

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
