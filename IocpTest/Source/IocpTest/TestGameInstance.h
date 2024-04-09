// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IocpTest.h"
#include "Engine/GameInstance.h"
#include "TestGameInstance.generated.h"

class FPacketSession;
class AIocpBaseCharacter;

class UUserChatWidget;
/**
 * 
 */
UCLASS()
class IOCPTEST_API UTestGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	/*
	간편한 구현을 위해 실행을 블루프린트에서 담당하도록 한다.
	어디에서 동작할지는 함수 위에 써놓음.
	*/

	//beginplay
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	//endplay
	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	//tick
	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	//플레이어 캐릭터에 의해 Chat UI 생성
	UFUNCTION(BlueprintCallable)
	void AddUserChatWidget();

	void SendPacket(SendBufferRef SendBuffer);

public:
	void HandleSpawn(const Protocol::ObjectInfo& ObjectInfo, bool bIsMyPlayer);
	void HandleSpawn(const Protocol::S2C_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S2C_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S2C_DESPAWN& DespawnPkt);

	void HandleMove(const Protocol::S2C_MOVE& MovePkt);
	void HandleChat(const Protocol::S2C_CHAT& ChatPkt);
	void HandleJump(const Protocol::S2C_JUMP& JumpPkt);
public:

	// GameServer
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<FPacketSession> GameServerSession;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AIocpBaseCharacter> OtherPlayerBP;

	AIocpBaseCharacter* MyIocpCharacter;
	TMap<uint64, AIocpBaseCharacter*> Players;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserChatWidget> UserChatBP;

	UPROPERTY()
	UUserChatWidget* UserChatWidget;
};
