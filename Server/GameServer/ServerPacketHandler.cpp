#include "pch.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

	//어떤 패킷 id가 들어왔는지 체크?

	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	//GameSessionRef gameSession = static_pointer_cast<FGameSession>(session);

	//// 원래는 Validation 체크 해야함.
	//
	//Protocol::S_LOGIN loginPkt;
	//loginPkt.set_success(true);

	//// DB에서 플레이 정보를 긁어온다고 가정함
	//// GameSession에 플레이 정보를 저장 (메모리에 저장될 것)

	////id 발급(db id가 아니라 인게임 id)
	//static Atomic<uint64> idGenerator = 1;

	//{
	//	auto player = loginPkt.add_players();
	//	player->set_name(u8"DB얻어온이름1");
	//	player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);

	//	PlayerRef playerRef = MakeShared<FPlayer>();
	//	playerRef->PlayerId = idGenerator++;
	//	playerRef->Name = player->name();
	//	playerRef->Type = player->playertype();
	//	playerRef->OwnerSession = gameSession;

	//	gameSession->Players.push_back(playerRef);
	//}

	//{
	//	auto player = loginPkt.add_players();
	//	player->set_name(u8"DB얻어온이름2");
	//	player->set_playertype(Protocol::PLAYER_TYPE_MAGE);

	//	PlayerRef playerRef = MakeShared<FPlayer>();
	//	playerRef->PlayerId = idGenerator++;
	//	playerRef->Name = player->name();
	//	playerRef->Type = player->playertype();
	//	playerRef->OwnerSession = gameSession;

	//	gameSession->Players.push_back(playerRef);
	//}

	//auto sendBuffer = FServerPacketHandler::MakeSendBuffer(loginPkt);
	//session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	//GameSessionRef gameSession = static_pointer_cast<FGameSession>(session);

	//uint64 index = pkt.playerindex();
	//// TODO : Validation
	////예시 : 게임 접속했는데 또 로그인 시도를 하려고 한다라던지
	////예시2 : 게임 플레이 중인데 또 로그인 시도를 하려고 한다라던지
	////... 등의 패킷 조작관련 해킹에 관한 처리를 다 해줘야함.

	////PlayerRef player = gameSession->Players[index]; // 아직 READ_ONLY라서 lock과는 상관없다.
	//
	////세션의 플레이어와 Room 등록
	//gameSession->CurrentPlayer = gameSession->Players[index];
	//gameSession->Room = GRoom;

	////!?
	//PlayerRef player = gameSession->CurrentPlayer;


	////설명 : Room은 JobQueue를 상속받아 일감을 처리한다.
	////일단 받은 일감인 FRoom::Enter를 JobQueue에 Push한다.(실제 일처리는 나중에 한다는 의미)
	////GameServer.h의 Dispatch를 빠져나오면 Global Queue를 실행함.
	////Push를 보면 실행중인 Jobqueue 확인하고 일처리할지 아니면 다른 쓰레드에게 넘길지 선택
	////그 이후엔 시간제한만큼 알아서 동작함.
	//GRoom->DoAsync(&FRoom::Enter, player);

	//Protocol::S_ENTER_GAME enterGamePkt;
	//enterGamePkt.set_success(true);
	//auto sendBuffer = FServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//player->OwnerSession->Send(sendBuffer);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	////cout << pkt.msg() << endl;
	//cout << "received c_chat" << endl;

	////모든 클라이언트에게 전달.
	//Protocol::S_CHAT chatPkt;
	////chatPkt.set_msg(pkt.msg());
	//chatPkt.set_msg(u8"broadcasted msg.. Hello World!");
	//auto sendBuffer = FServerPacketHandler::MakeSendBuffer(chatPkt);

	//GRoom->DoAsync(&FRoom::Broadcast, sendBuffer);

	return true;
}
