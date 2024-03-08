// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameInstance.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "IocpBaseCharacter.h"

#include "Blueprint/UserWidget.h"
#include "ChatSlotWidget.h"
#include "UserChatWidget.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"


void UTestGameInstance::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(
		PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"),
			TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr =
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(
		-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Connecting To Server...")));

	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		// Session
		GameServerSession = MakeShared<FPacketSession>(Socket);
		GameServerSession->Run();


		// TEMP : Lobby에서 캐릭터 선택창 등
		{
			Protocol::C_LOGIN pkt;
			SendBufferRef sendBuffer 
				= FClientPacketHandler::MakeSendBuffer(pkt);
			SendPacket(sendBuffer);
		}

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 5.f, FColor::Red,
			FString::Printf(TEXT("Connection Failed")));
	}
}

void UTestGameInstance::DisconnectFromGameServer()
{

	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	Protocol::C_LEAVE_GAME LeavePkt;
	SEND_PACKET(LeavePkt);

	//if (Socket)
	//{
	//	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
	//	SocketSubsystem->DestroySocket(Socket);
	//	Socket = nullptr;
	//}

}

void UTestGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UTestGameInstance::AddUserChatWidget()
{
	if (!IsValid(UserChatBP))
	{
		return;
	}

	UserChatWidget
	= CreateWidget<UUserChatWidget>(this, UserChatBP);

	UserChatWidget->AddToViewport();
}

void UTestGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UTestGameInstance::HandleSpawn(const Protocol::ObjectInfo& ObjectInfo, bool bIsMyPlayer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	// 중복 처리 체크
	const uint64 objectId = ObjectInfo.object_id();
	if (Players.Find(objectId) != nullptr)
	{
		return; //중복이 왜 있는지는 모르지만 끝낸다.
	}
	
	FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());

	if (bIsMyPlayer)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		AIocpBaseCharacter* iocpCharacter = Cast<AIocpBaseCharacter>(PC->GetPawn());
		if (iocpCharacter == nullptr)
		{
			return;
		}

		//spawn시 위치 세팅
		iocpCharacter->SetPlayerInfo(ObjectInfo.pos_info());

		MyIocpCharacter = iocpCharacter;
		Players.Add(ObjectInfo.object_id(), iocpCharacter);
	}
	else
	{
		AActor* Actor = world->SpawnActor(OtherPlayerBP, &SpawnLocation);
		AIocpBaseCharacter* iocpCharacter = Cast<AIocpBaseCharacter>(Actor);
	
		iocpCharacter->SetPlayerInfo(ObjectInfo.pos_info());
		
		Players.Add(ObjectInfo.object_id(), iocpCharacter);
	}


}

void UTestGameInstance::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true); //플레이어의 캐릭터
}

void UTestGameInstance::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (const Protocol::ObjectInfo& Player : SpawnPkt.players())
	{
		HandleSpawn(Player, false); // 다른 플레이어의 캐릭터
	}
}

void UTestGameInstance::HandleDespawn(uint64 ObjectId)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	AIocpBaseCharacter** foundActor = Players.Find(ObjectId);
	if (foundActor == nullptr)
		return;

	World->DestroyActor(*foundActor);
}

void UTestGameInstance::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (auto& ObjectId : DespawnPkt.object_ids())
	{
		HandleDespawn(ObjectId);
	}
}

void UTestGameInstance::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 objectId = MovePkt.info().object_id();

	AIocpBaseCharacter** foundActor = Players.Find(objectId);
	if (foundActor == nullptr)
		return;

	AIocpBaseCharacter* Player = (*foundActor);
	
	//패킷을 계속 broadcast하기 때문에 과거의 나의 위치를 보내면
	//서버가 그 패킷 정보로 다시 갱신되서 플레이어가 제자리 걸음을 함
	//그 부분을 막기 위해서 S_Move 패킷은 클라이언트의 플레이어는 제외함.
	if (Player->IsMyCharacter())
		return;

	const Protocol::PosInfo& Info = MovePkt.info();
	
	//목적지로 보정 이동하는 로직을 실행
	//Player->SetPlayerInfo(Info);
	Player->SetDestInfo(Info);


}

void UTestGameInstance::HandleChat(const Protocol::S_CHAT& ChatPkt)
{
	if (!IsValid(UserChatWidget))
	{
		return;
	}

	if (!IsValid(ChatSlotBP))
	{
		return;
	}

	auto msg = ChatPkt.msg();
	FString str = *FString(msg.c_str());

	auto chatSlot = CreateWidget<UChatSlotWidget>(this, ChatSlotBP);
	chatSlot->TextBlock_Chat->SetText(FText::FromString(str));

	UserChatWidget->ScrollBox_ChatScroll->AddChild(chatSlot);

}
