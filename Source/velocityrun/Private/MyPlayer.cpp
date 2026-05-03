#include "MyPlayer.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetEnableGravity(false);

	bFlying = false;
}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}
}

void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ONLY UP / DOWN FLY
	if (bFlying)
	{
		FVector Up = FVector(0.f, 0.f, 1.f);
		GetCapsuleComponent()->AddForce(Up * FlyForce);
	}
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(FlyAction, ETriggerEvent::Started, this, &AMyPlayer::StartFly);
		Input->BindAction(FlyAction, ETriggerEvent::Completed, this, &AMyPlayer::StopFly);
	}
}

void AMyPlayer::StartFly()
{
	bFlying = true;
}

void AMyPlayer::StopFly()
{
	bFlying = false;
}