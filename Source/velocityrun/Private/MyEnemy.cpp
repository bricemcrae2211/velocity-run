#include "MyEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AMyEnemy::AMyEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));

	GetCharacterMovement()->MaxWalkSpeed = 999999.f;

	// ================= PHYSICS MODE =================
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetEnableGravity(true);
	GetCapsuleComponent()->SetLinearDamping(2.5f);
	GetCapsuleComponent()->SetAngularDamping(10.f);

	// ================= FACE ONCE FLAG =================
	bHasFacedPlayer = false;
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

	float ZDiff = FMath::Abs(GetActorLocation().Z - Player->GetActorLocation().Z);
	bPlayerSameLevel = (ZDiff <= ZTolerance);

	// ================= FRONT CHECK (ADDED) =================
	FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.f;

	FVector Forward = GetActorForwardVector();
	Forward.Z = 0.f;

	ToPlayer.Normalize();
	Forward.Normalize();

	float Dot = FVector::DotProduct(Forward, ToPlayer);
	bPlayerInFront = Dot > FrontDotThreshold;

	// ================= ENTER RANGE =================
	if (!bPlayerInRange && Distance <= DetectionRange)
	{
		bPlayerInRange = true;

		const FVector PlayerVel = Player->GetVelocity();
		CurrentSpeed = PlayerVel.Size();

		if (CurrentSpeed < 500.f)
			CurrentSpeed = 500.f;

		bSpeedInitialized = true;
	}

	// ================= FACE ONLY ONCE =================
	if (bPlayerInRange && !bHasFacedPlayer)
	{
		FacePlayer();
		bHasFacedPlayer = true;
	}

	UpdateMovement(DeltaTime);
}

void AMyEnemy::FacePlayer()
{
	if (!Player) return;

	FVector Dir = Player->GetActorLocation() - GetActorLocation();
	Dir.Z = 0.f;

	if (Dir.IsNearlyZero()) return;

	FRotator Rot = Dir.Rotation();
	Rot.Pitch = 0.f;
	Rot.Roll = 0.f;

	SetActorRotation(Rot);
}

void AMyEnemy::UpdateMovement(float DeltaTime)
{
	if (!bPlayerInRange) return;

	UPrimitiveComponent* Body = Cast<UPrimitiveComponent>(GetRootComponent());
	if (!Body) return;

	FVector CurrentVel = Body->GetPhysicsLinearVelocity();

	FVector MoveDir;

	float TargetSpeed;

	// ================= SPEED LOGIC SWAP =================
	if (bMoveForwardInsteadOfBackward)
	{
		// FORWARD = FIXED SPEED 500
		MoveDir = GetActorForwardVector();
		TargetSpeed = 500.f;
	}
	else
	{
		// BACKWARD = PLAYER SPEED
		MoveDir = -GetActorForwardVector();
		TargetSpeed = CurrentSpeed;
	}

	MoveDir.Z = 0.f;
	MoveDir.Normalize();

	FVector TargetVel = MoveDir * TargetSpeed;
	TargetVel.Z = CurrentVel.Z;

	FVector Force = (TargetVel - CurrentVel) * 8.0f;

	Body->AddForce(Force, NAME_None, true);

	// ================= ONLY DECAY PLAYER SPEED =================
	if (!bMoveForwardInsteadOfBackward)
	{
		CurrentSpeed -= DecayRate * DeltaTime;
		CurrentSpeed = FMath::Max(CurrentSpeed, 0.f);
	}
}