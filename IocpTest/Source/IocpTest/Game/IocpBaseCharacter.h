// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "protocol.pb.h"
#include "GameFramework/Character.h"
#include "IocpBaseCharacter.generated.h"

UCLASS()
class IOCPTEST_API AIocpBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIocpBaseCharacter();
	virtual ~AIocpBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	bool IsMyCharacter();

	Protocol::MoveState GetMoveState() { return PlayerInfo->state(); }
	void SetMoveState(Protocol::MoveState State);

public:
	void SetPlayerInfo(const Protocol::PosInfo& Info);
	void SetDestInfo(const Protocol::PosInfo& Info);
	Protocol::PosInfo* GetPlayerInfo() { return PlayerInfo; }

protected:
	class Protocol::PosInfo* PlayerInfo = nullptr; // 현재 위치
	class Protocol::PosInfo* DestInfo = nullptr; // 다른 플레이어들의 목적지 위치
};
