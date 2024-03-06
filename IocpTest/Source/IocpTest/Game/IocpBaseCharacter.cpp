// Fill out your copyright notice in the Description page of Project Settings.


#include "IocpBaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IocpMyCharacter.h"

// Sets default values
AIocpBaseCharacter::AIocpBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//GetCharacterMovement()->bRunPhysicsWithNoController = true;

	PlayerInfo = new Protocol::PlayerInfo();
}

AIocpBaseCharacter::~AIocpBaseCharacter()
{
	delete PlayerInfo;
	PlayerInfo = nullptr;
}

// Called when the game starts or when spawned
void AIocpBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIocpBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//캐릭터들의 정보(위치)값을 지속적으로 갱신함.
	{
		FVector Location = GetActorLocation();
		PlayerInfo->set_x(Location.X);
		PlayerInfo->set_y(Location.Y);
		PlayerInfo->set_z(Location.Z);
		PlayerInfo->set_yaw(GetControlRotation().Yaw);
	}

}

bool AIocpBaseCharacter::IsMyCharacter()
{
	//myCharacter로 캐스팅해서 체크함.
	return Cast<AIocpMyCharacter>(this) != nullptr;
}

void AIocpBaseCharacter::SetPlayerInfo(const Protocol::PlayerInfo& Info)
{
	if (PlayerInfo->object_id() != 0) //object id를 변경하려는 시도가 있으면 비정상적임
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}

	//@!#$
	PlayerInfo->CopyFrom(Info);


	FVector Location(Info.x(), Info.y(), Info.z());
	SetActorLocation(Location);
}

