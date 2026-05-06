#include "MyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AMyEnemy::AMyEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));

	GetCharacterMovement()->MaxWalkSpeed = 999999.f;
}

void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player) return;

	// ================= RANGE CHECK =================
	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

	// ?? ADDED: SAME Z LEVEL CHECK
	float ZDiff = FMath::Abs(GetActorLocation().Z - Player->GetActorLocation().Z);
	bPlayerSameLevel = (ZDiff <= ZTolerance);

	// enter range once
	if (!bPlayerInRange && Distance <= DetectionRange)
	{
		bPlayerInRange = true;

		// ?? KEY FIX: COPY PLAYER SPEED ON ENTRY
		const FVector PlayerVel = Player->GetVelocity();
		CurrentSpeed = PlayerVel.Size();

		if (CurrentSpeed < 500.f)
		{
			CurrentSpeed = 500.f; // fallback
		}

		bSpeedInitialized = true;
	}

	FacePlayer();
	UpdateMovement(DeltaTime);
}

void AMyEnemy::FacePlayer()
{
	if (!Player) return;

	FVector Dir = Player->GetActorLocation() - GetActorLocation();
	Dir.Y = 0.f;

	FRotator Rot = Dir.Rotation();
	Rot.Pitch = 0.f;
	Rot.Roll = 0.f;

	SetActorRotation(Rot);
}

void AMyEnemy::UpdateMovement(float DeltaTime)
{
	if (!bPlayerInRange || !bSpeedInitialized)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	// ================= MOVE BACKWARD =================
	FVector BackDir = -GetActorForwardVector();
	FVector Velocity = BackDir * CurrentSpeed;

	// ================= DECAY SPEED =================
	CurrentSpeed -= DecayRate * DeltaTime;

	if (CurrentSpeed < 0.f)
	{
		CurrentSpeed = 0.f;
	}

	GetCharacterMovement()->Velocity = Velocity;
}