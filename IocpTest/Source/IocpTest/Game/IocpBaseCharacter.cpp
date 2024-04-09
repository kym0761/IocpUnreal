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


	PlayerInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();

	//aicontroller possess할 것이라 체크하지 않음.
	// BP에서 세팅함
	// C++에선 //AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	//GetCharacterMovement()->bRunPhysicsWithNoController = true;
}

AIocpBaseCharacter::~AIocpBaseCharacter()
{
	delete PlayerInfo;
	PlayerInfo = nullptr;

	delete DestInfo;
	DestInfo = nullptr;
}

// Called when the game starts or when spawned
void AIocpBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//패킷에 맞춰 캐릭터의 위치를 설정함
	{
		FVector Location = GetActorLocation();
		DestInfo->set_x(Location.X);
		DestInfo->set_y(Location.Y);
		DestInfo->set_z(Location.Z);
		DestInfo->set_yaw(GetControlRotation().Yaw);

		SetMoveState(Protocol::MOVE_STATE_IDLE);
	}

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

	if (!IsMyCharacter())
	{
		
		const Protocol::MoveState state = PlayerInfo->state();

		if (state == Protocol::MOVE_STATE_RUN)
		{
			SetActorRotation(FRotator(0, DestInfo->yaw(), 0));


			// 플레이어가 조종하지 않은 경우, run physics with no controller 옵션이 켜져야함.
			//근데 사실 possess 세팅을 spawn & place world해서 aicontroller가 붙으면 딱히 문제 없다.
			AddMovementInput(GetActorForwardVector());
		}


	}

}

bool AIocpBaseCharacter::IsMyCharacter()
{
	//myCharacter로 캐스팅해서 체크함.
	return Cast<AIocpMyCharacter>(this) != nullptr;
}

void AIocpBaseCharacter::SetMoveState(Protocol::MoveState State)
{
	//같은 상태면 스킵함.
	if (PlayerInfo->state() == State)
		return;

	PlayerInfo->set_state(State);
}

void AIocpBaseCharacter::SetPlayerInfo(const Protocol::PosInfo& Info)
{
	//if (PlayerInfo == nullptr)
	//{
	//	return;
	//}


	if (PlayerInfo->object_id() != 0) //object id를 변경하려는 시도가 있으면 비정상적임
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}

	//@!#$
	PlayerInfo->CopyFrom(Info);


	FVector Location(Info.x(), Info.y(), Info.z());
	FRotator rotation(0.0f, PlayerInfo->yaw(), 0.0f);
	SetActorLocation(Location);
	SetActorRotation(rotation);
}

void AIocpBaseCharacter::SetDestInfo(const Protocol::PosInfo& Info)
{
	//if (PlayerInfo == nullptr || DestInfo == nullptr)
	//{
	//	return;
	//}
	
	if (PlayerInfo->object_id() != 0) //object id를 변경하려는 시도가 있으면 비정상적임
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}


	//@!#$
	DestInfo->CopyFrom(Info);

	//상태는 즉시 적용
	SetMoveState(Info.state());
}

