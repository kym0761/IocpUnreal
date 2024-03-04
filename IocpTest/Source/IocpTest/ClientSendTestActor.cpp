// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSendTestActor.h"
#include "Protocol.pb.h"
#include "TestGameInstance.h"
#include "ClientPacketHandler.h"

// Sets default values
AClientSendTestActor::AClientSendTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClientSendTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClientSendTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	Protocol::C_CHAT Msg;
	Msg.set_msg("Hello Server I am Unreal Client.");
	SendBufferRef SendBuffer = FClientPacketHandler::MakeSendBuffer(Msg);
	Cast<UTestGameInstance>(GetGameInstance())->SendPacket(SendBuffer);

}

