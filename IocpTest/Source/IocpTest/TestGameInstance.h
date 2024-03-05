// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IocpTest.h"
#include "Engine/GameInstance.h"
#include "TestGameInstance.generated.h"

class FPacketSession;

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

	void SendPacket(SendBufferRef SendBuffer);

public:
	void HandleSpawn(const Protocol::PlayerInfo& PlayerInfo);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

public:

	// GameServer
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<FPacketSession> GameServerSession;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlayerBP;

	TMap<uint64, AActor*> Players;

};
