#include "pch.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "ObjectUtils.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

	//어떤 패킷 id가 들어왔는지 체크?

	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	//DB에서 Account 정보를 얻는 것을 가정함.
	//DB에서 유저의 캐릭터 정보를 얻어오는 것을 가정함.
	Protocol::S_LOGIN loginPkt;

	//플레이어의 캐릭터가 3개가 있을 것이라고 가정함.
	for (int32 i = 0; i < 3; i++)
	{
		Protocol::ObjectInfo* player = loginPkt.add_players();

		Protocol::PosInfo* posInfo = player->mutable_pos_info();

		posInfo->set_x(FServerUtils::GetRandom(0.f, 100.f));
		posInfo->set_y(FServerUtils::GetRandom(0.f, 100.f));
		posInfo->set_z(FServerUtils::GetRandom(0.f, 100.f));
		posInfo->set_yaw(FServerUtils::GetRandom(0.f, 45.f));
	}

	loginPkt.set_success(true);
	SEND_PACKET(loginPkt);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	// 플레이어 생성
	PlayerRef player = 
		FObjectUtils::CreatePlayer(
			static_pointer_cast<FGameSession>(session));

	// //방에 입장
	//GRoom->HandleEnterPlayerLocked(player);

	//플레이어가 방에 입장하는 것을 예약함.
	GRoom->DoAsync(&FRoom::HandleEnterPlayer, player);

	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->Room.load().lock();
	if (room == nullptr)
		return false;

	//room->HandleLeavePlayerLocked(player);

	GRoom->DoAsync(&FRoom::HandleLeavePlayer, player);

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->Room.load().lock();
	if (room == nullptr)
		return false;

	// TODO

	//room->HandleMoveLocked(pkt);

	room->DoAsync(&FRoom::HandleMove, pkt);

	return true;
}


bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	cout << pkt.msg() << endl;

	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
	{
		return false;
	}
		
	GRoom->DoAsync(&FRoom::HandleChatFromPlayer, player, pkt);

	return true;
}
