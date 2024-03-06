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

public:
	void SetPlayerInfo(const Protocol::PlayerInfo& Info);
	//void SetDestInfo(const Protocol::PlayerInfo& Info);
	Protocol::PlayerInfo* GetPlayerInfo() { return PlayerInfo; }

protected:
	class Protocol::PlayerInfo* PlayerInfo; // 현재 위치

};
