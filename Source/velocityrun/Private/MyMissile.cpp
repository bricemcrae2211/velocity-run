#include "MyMissile.h"
#include "MyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

AMyMissile::AMyMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	PhysicsBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PhysicsBody"));
	SetRootComponent(PhysicsBody);

	PhysicsBody->SetSimulatePhysics(false);
	PhysicsBody->SetEnableGravity(false);
}

void AMyMissile::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<AMyPlayer>(
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0)
	);
}

void AMyMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerRef)
		return;

	// ================= HELD =================
	if (IsHeld)
	{
		PhysicsBody->SetSimulatePhysics(true);
		PhysicsBody->SetEnableGravity(true);
		return;
	}

	// ================= ALWAYS MATCH PLAYER SPEED =================
	// THIS IS THE IMPORTANT PART
	float PlayerSpeed = PlayerRef->CurrentMaxSpeed;

	// ================= DISTANCE CHECK =================
	float Distance = FVector::Dist(
		GetActorLocation(),
		PlayerRef->GetActorLocation()
	);

	if (!bTriggered && Distance <= TriggerDistance)
	{
		bTriggered = true;

		PrintInRange();

		FVector Dir =
			PlayerRef->GetActorLocation() - GetActorLocation();

		Dir.Z = 0.f;

		SetActorRotation(Dir.Rotation());

		bMovingBackward = true;

		GetWorldTimerManager().SetTimer(
			LaunchTimer,
			this,
			&AMyMissile::StartForwardLaunch,
			LaunchDelay,
			false
		);
	}

	// ================= MOVE BACKWARD =================
	// EXACT SAME SPEED AS PLAYER
	if (bMovingBackward && !bLaunched)
	{
		FVector Move =
			-GetActorForwardVector() *
			PlayerSpeed *
			DeltaTime;

		AddActorWorldOffset(Move, true);
	}

	// ================= LAUNCH FORWARD =================
	// EXACT SAME SPEED AS PLAYER
	if (bLaunched)
	{
		FVector Move =
			GetActorForwardVector() *
			PlayerSpeed *
			DeltaTime;

		AddActorWorldOffset(Move, true);
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