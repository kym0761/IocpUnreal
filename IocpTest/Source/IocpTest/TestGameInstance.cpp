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

void UTestGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UTestGameInstance::HandleSpawn(const Protocol::PlayerInfo& PlayerInfo, bool bIsMyPlayer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	// 중복 처리 체크
	const uint64 objectId = PlayerInfo.object_id();
	if (Players.Find(objectId) != nullptr)
	{
		return; //중복이 왜 있는지는 모르지만 끝낸다.
	}
	
	FVector SpawnLocation(PlayerInfo.x(), PlayerInfo.y(), PlayerInfo.z());

	if (bIsMyPlayer)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		AIocpBaseCharacter* iocpCharacter = Cast<AIocpBaseCharacter>(PC->GetPawn());
		if (iocpCharacter == nullptr)
		{
			return;
		}
		MyIocpCharacter = iocpCharacter;
		Players.Add(PlayerInfo.object_id(), iocpCharacter);
	}
	else
	{
		AActor* Actor = world->SpawnActor(OtherPlayerBP, &SpawnLocation);
		AIocpBaseCharacter* iocpCharacter = Cast<AIocpBaseCharacter>(Actor);
		Players.Add(PlayerInfo.object_id(), iocpCharacter);
	}


}

void UTestGameInstance::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true);
}

void UTestGameInstance::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (const Protocol::PlayerInfo& Player : SpawnPkt.players())
	{
		HandleSpawn(Player, false);
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
