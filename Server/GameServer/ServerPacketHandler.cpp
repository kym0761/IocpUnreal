#include "pch.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "ObjectUtils.h"

//66535개인데, 초과할 일이 있는지는 모르겠다.
PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

	//어떤 패킷 id가 들어왔는지 체크?

	return false;
}

bool Handle_C2S_LOGIN(PacketSessionRef& session, Protocol::C2S_LOGIN& pkt)
{
	//DB에서 Account 정보를 얻는 것을 가정함.
	//DB에서 유저의 캐릭터 정보를 얻어오는 것을 가정함.


	Protocol::S2C_LOGIN loginPkt;

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

bool Handle_C2S_ENTER_GAME(PacketSessionRef& session, Protocol::C2S_ENTER_GAME& pkt)
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

bool Handle_C2S_LEAVE_GAME(PacketSessionRef& session, Protocol::C2S_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
	{
		return false;
	}

	RoomRef room = player->Room.load().lock();
	if (room == nullptr)
	{
		return false;
	}

	//room->HandleLeavePlayerLocked(player);

	GRoom->DoAsync(&FRoom::HandleLeavePlayer, player);

	return true;
}

bool Handle_C2S_MOVE(PacketSessionRef& session, Protocol::C2S_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
	{
		return false;
	}

	RoomRef room = player->Room.load().lock();
	if (room == nullptr)
	{
		return false;
	}

	// TODO : 올바른 이동 값인지 체크할 필요도 있을 것

	//room->HandleMoveLocked(pkt);

	room->DoAsync(&FRoom::HandleMove, pkt);

	return true;
}

bool Handle_C2S_JUMP(PacketSessionRef& session, Protocol::C2S_JUMP& pkt)
{
	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
	{
		return false;
	}

	RoomRef room = player->Room.load().lock();
	if (room == nullptr)
	{
		return false;
	}

	uint64 obj_id= player->ObjectInfo->object_id();

	room->DoAsync(&FRoom::HandleJump, obj_id);

	return false;
}


bool Handle_C2S_CHAT(PacketSessionRef& session, Protocol::C2S_CHAT& pkt)
{
	auto gameSession = static_pointer_cast<FGameSession>(session);

	PlayerRef player = gameSession->Player.load();
	if (player == nullptr)
	{
		return false;
	}

	//서버에서 클라이언트들의 ChatMsg 확인 용도 및 로그 파일 추가
	{
		string s = pkt.msg();

		//protobuf가 utf-8만 지원하는 관계로 원래 의도인 utf-16으로 변환해야함.
		//utf-8 -> utf-16
		//1. 길이를 잰다.
		//2. 길이에 맞춰 wstring으로 변환한다.
		int nLen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.size(), NULL, NULL);
		wstring ws(nLen, 0);
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.size(), ws.data(), nLen);

		//날짜 및 시간
		std::time_t now = std::time(nullptr);
		tm tm01;

		localtime_s(&tm01, &now);

		//날짜 및 시간 파싱
		wstring date = to_wstring(tm01.tm_year + 1900) + L"." +
			to_wstring(tm01.tm_mon + 1) + L"." +
			to_wstring(tm01.tm_mday) + L"_" +
			to_wstring(tm01.tm_hour) + L":" +
			to_wstring(tm01.tm_min) + L":" +
			to_wstring(tm01.tm_sec);

		//[날짜 및 시간] 플레이어 이름 : 채팅 내용
		ws = L"[" + date + L"]" + L"\t\t" + L"Player " + to_wstring(player->ObjectInfo->object_id()) + L" : " + ws;

		wcout.imbue(locale("kor"));
		wcout << ws << endl;

		//텍스트 파일에 채팅 로그 저장.
		wofstream fout;
		fout.imbue(locale("kor"));

		fout.open("ChatLog" + GRoom->ServerStartTime + ".txt", ios::out | ios::app);
		fout << ws;
		fout << "\n";
	}

	GRoom->DoAsync(&FRoom::HandleChatFromPlayer, player, pkt);

	return true;
}
