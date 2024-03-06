#include "ClientPacketHandler.h"
#include "BufferReader.h"

#include "IocpTest.h"
#include "TestGameInstance.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	//예시 1
	for (auto& Player : pkt.players())
	{
	}

	//예시 2
	for (int32 i = 0; i < pkt.players_size(); i++)
	{
		const Protocol::PlayerInfo& Player = pkt.players(i);
	}

	// 로비에서 캐릭터 선택했다고 가정함.
	Protocol::C_ENTER_GAME EnterGamePkt;
	EnterGamePkt.set_playerindex(0);
	SEND_PACKET(EnterGamePkt);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (UTestGameInstance* GameInstance 
		= Cast<UTestGameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->HandleSpawn(pkt);
	}

	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	if (UTestGameInstance* GameInstance
		= Cast<UTestGameInstance>(GWorld->GetGameInstance())) 
	{
		//게임을 아예 종료할 것인지? 아니면 메인 로비로 이동할 것인지? 선택해야함.


		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Red,
			FString::Printf(TEXT("Client Leave Game")));

	}

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	if (UTestGameInstance* GameInstance
		= Cast<UTestGameInstance>(GWorld->GetGameInstance())) 
	{
		GameInstance->HandleSpawn(pkt);
	}

	return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	if (UTestGameInstance* GameInstance
		= Cast<UTestGameInstance>(GWorld->GetGameInstance())) 
	{
		GameInstance->HandleDespawn(pkt);
	}

	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	if (UTestGameInstance* GameInstance
		= Cast<UTestGameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->HandleMove(pkt);
	}

	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	////받은 메시지 보여줌
	//std::cout << pkt.msg() << endl;

	auto Msg = pkt.msg();

	return true;

}

