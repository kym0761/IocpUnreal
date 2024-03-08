// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IocpTest.h"
#include "Engine/GameInstance.h"
#include "TestGameInstance.generated.h"

class FPacketSession;
class AIocpBaseCharacter;

class UChatSlotWidget;
class UUserChatWidget;
/**
 * 
 */
UCLASS()
class IOCPTEST_API UTestGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	//블루프린트에서 실행을 담당

	//beginplay
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	//endplay
	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	//tick
	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();


	UFUNCTION(BlueprintCallable)
	void AddUserChatWidget();

	void SendPacket(SendBufferRef SendBuffer);

public:
	void HandleSpawn(const Protocol::ObjectInfo& ObjectInfo, bool bIsMyPlayer);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleMove(const Protocol::S_MOVE& MovePkt);
	void HandleChat(const Protocol::S_CHAT& ChatPkt);

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
	TSubclassOf<UChatSlotWidget> ChatSlotBP;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserChatWidget> UserChatBP;

	UPROPERTY()
	UUserChatWidget* UserChatWidget;
};
