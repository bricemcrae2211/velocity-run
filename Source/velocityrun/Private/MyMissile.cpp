#include "MyMissile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

AMyMissile::AMyMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root = capsule (collision)
	SetRootComponent(CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root")));
}

void AMyMissile::BeginPlay()
{
	Super::BeginPlay();

	PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AMyMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerActor) return;

	UCapsuleComponent* Root = Cast<UCapsuleComponent>(GetRootComponent());
	if (!Root) return;

	// ================= HELD MODE =================
	if (IsHeld)
	{
		Root->SetSimulatePhysics(true);
		Root->SetEnableGravity(true);

		// IMPORTANT: stop AI movement completely
		return;
	}

	// ================= NOT HELD =================
	// DO NOTHING (as you requested)
	// AI continues normally below

	float Distance = FVector::Dist(GetActorLocation(), PlayerActor->GetActorLocation());

	// ================= TRIGGER =================
	if (!bTriggered && Distance <= TriggerDistance)
	{
		bTriggered = true;

		PrintInRange();

		FVector Direction = PlayerActor->GetActorLocation() - GetActorLocation();
		Direction.Z = 0.f;

		SetActorRotation(Direction.Rotation());

		bMovingBackward = true;

		GetWorldTimerManager().SetTimer(
			LaunchTimer,
			this,
			&AMyMissile::StartForwardLaunch,
			LaunchDelay,
			false
		);
	}

	// ================= BACKWARD =================
	if (bMovingBackward && !bLaunched)
	{
		AddActorWorldOffset(
			-GetActorForwardVector() * BackwardSpeed * DeltaTime,
			true
		);
	}

	// ================= FORWARD =================
	if (bLaunched)
	{
		AddActorWorldOffset(
			GetActorForwardVector() * ForwardSpeed * DeltaTime,
			true
		);
	}
}

void AMyMissile::StartForwardLaunch()
{
	bMovingBackward = false;
	bLaunched = true;
}

void AMyMissile::PrintInRange()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Red,
			TEXT("PLAYER IN RANGE")
		);
	}
}