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
#include "UserChatWidget.h"


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

	//언리얼 엔진 FSocket은 블로킹으로만 구성되어 있다.
	//연결이 되기 전까지 프로그램이 멈출 것이다.
	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		// Session
		GameServerSession = MakeShared<FPacketSession>(Socket);
		GameServerSession->Run();


		// TEMP : Lobby에서 캐릭터 선택창 등
		{
			Protocol::C2S_LOGIN pkt;
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

	Protocol::C2S_LEAVE_GAME LeavePkt;
	SEND_PACKET(LeavePkt);
}

void UTestGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UTestGameInstance::AddUserChatWidget()
{
	//assert(UserChatBP);
	if (!IsValid(UserChatBP))
	{
		return;
	}

	UserChatWidget = CreateWidget<UUserChatWidget>(this, UserChatBP);

	if (IsValid(UserChatWidget))
	{
		UserChatWidget->AddToViewport();
	}
	
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
	
	//Spawn에 필요한 Location Rotation 정보
	FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());
	FRotator spawnRotation(0.0f, ObjectInfo.pos_info().yaw(), 0.0f);

	if (bIsMyPlayer)
	{
		auto PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC == nullptr)
		{
			return;
		}

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
		AActor* Actor = world->SpawnActor(OtherPlayerBP, &SpawnLocation, &spawnRotation);
		AIocpBaseCharacter* iocpCharacter = Cast<AIocpBaseCharacter>(Actor);
	
		//iocpCharacter->SetPlayerInfo(ObjectInfo.pos_info());
		
		Players.Add(ObjectInfo.object_id(), iocpCharacter);
	}


}

void UTestGameInstance::HandleSpawn(const Protocol::S2C_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true); //플레이어의 캐릭터
}

void UTestGameInstance::HandleSpawn(const Protocol::S2C_SPAWN& SpawnPkt)
{
	for (const Protocol::ObjectInfo& Player : SpawnPkt.players())
	{
		HandleSpawn(Player, false); // 다른 플레이어의 캐릭터
	}
}

void UTestGameInstance::HandleDespawn(uint64 ObjectId)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}
		
	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
		
	AIocpBaseCharacter** foundActor = Players.Find(ObjectId);
	if (foundActor == nullptr)
	{
		return;
	}

	World->DestroyActor(*foundActor);
}

void UTestGameInstance::HandleDespawn(const Protocol::S2C_DESPAWN& DespawnPkt)
{
	for (auto& ObjectId : DespawnPkt.object_ids())
	{
		HandleDespawn(ObjectId);
	}
}

void UTestGameInstance::HandleMove(const Protocol::S2C_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
		

	const uint64 objectId = MovePkt.info().object_id();

	AIocpBaseCharacter** foundActor = Players.Find(objectId);
	if (foundActor == nullptr)
	{
		return;
	}
		
	AIocpBaseCharacter* Player = (*foundActor);
	
	//패킷을 계속 broadcast하기 때문에 과거의 나의 위치를 보내면
	//서버가 그 패킷 정보로 다시 갱신되서 플레이어가 제자리 걸음을 함
	//그 부분을 막기 위해서 S2C_Move 패킷은 클라이언트의 플레이어는 제외함.
	//if (Player->IsMyCharacter())
	//{
	//	return;
	//}


	//if (Player->IsMyCharacter())
	//{
	//	FVector playerLocation = Player->GetActorLocation();
	//	FVector compareLocation = FVector(MovePkt.info().x(), MovePkt.info().y(), MovePkt.info().z());
	//	
	//	//콜리전 등의 이유로 위치가 잘못되는 부분을 위해서 보정함.
	//	if (FVector::Distance(playerLocation, compareLocation) < 10.0f)
	//	{
	//		return;
	//	}
	//	
	//}

		
	const Protocol::PosInfo& Info = MovePkt.info();
	
	//Player->SetPlayerInfo(Info);
	
	//목적지로 보정 이동하는 로직을 실행
	Player->SetDestInfo(Info);


}

void UTestGameInstance::HandleChat(const Protocol::S2C_CHAT& ChatPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
		

	if (!IsValid(UserChatWidget))
	{
		return;
	}

	auto msg = ChatPkt.msg();
	FString str = *FString(UTF8_TO_TCHAR(msg.c_str())); //utf-8 에서 utf-16

	UserChatWidget->AddChat(str);
}

void UTestGameInstance::HandleJump(const Protocol::S2C_JUMP& JumpPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	auto World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const uint64 objectId = JumpPkt.playerid();

	auto playerCharacter = Players[objectId];
	if (IsValid(playerCharacter)) //&& !playerCharacter->IsMyCharacter()) // 서버 쪽에서 이미 S2C Jump 보낼때 Broadcast에서 점프를 실행한 클라이언트 쪽의 플레이어 점프를 제외시켰다.
	{
		playerCharacter->Jump();
	}
}
