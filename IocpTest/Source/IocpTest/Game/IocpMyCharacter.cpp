// Fill out your copyright notice in the Description page of Project Settings.


#include "IocpMyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "IocpTest.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AIocpMyCharacter::AIocpMyCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

}

// Called when the game starts or when spawned
void AIocpMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

// Called every frame
void AIocpMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//어떠한 상황이 발생해서 바로 Send해야하는지 확인
	bool bForceSendPacket = false;

	if (LastDesiredInput != DesiredInput)
	{
		bForceSendPacket = true;
		LastDesiredInput = DesiredInput;
	}

	// State 정보
	if (DesiredInput == FVector2D::Zero())
	{
		SetMoveState(Protocol::MOVE_STATE_IDLE);
	}
	else
	{
		SetMoveState(Protocol::MOVE_STATE_RUN);
	}

	MovePacketSendTimer -= DeltaTime;

	if (MovePacketSendTimer <= 0 || bForceSendPacket == true)
	{
		MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

		Protocol::C_MOVE MovePkt;

		{
			Protocol::PosInfo* mutable_Info = MovePkt.mutable_info(); //수정가능한 info
			mutable_Info->CopyFrom(*PlayerInfo); //먼저, 데이터는 그대로 복사

			//state
			mutable_Info->set_state(GetMoveState());

			//yaw
			mutable_Info->set_yaw(DesiredYaw);
		}

		SEND_PACKET(MovePkt);
	}

}

// Called to bind functionality to input
void AIocpMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIocpMyCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AIocpMyCharacter::Move); //wasd를 떼서 이동안할 때도 이를 동작시켜야함.
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIocpMyCharacter::Look);
	}
	else
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."),
			*GetNameSafe(this));
	}

}

void AIocpMyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		//캐시 데이터 갱신
		{
			DesiredInput = MovementVector;

			DesiredMoveDirection = FVector::ZeroVector;
			DesiredMoveDirection += ForwardDirection * MovementVector.Y;
			DesiredMoveDirection += RightDirection * MovementVector.X;
			DesiredMoveDirection.Normalize();

			const FVector Location = GetActorLocation();
			FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
			DesiredYaw = Rotator.Yaw;
		}

	}
}

void AIocpMyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

