#include "MyPlayer.h"


#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyMissile.h"
#include "TimerManager.h"
#include "EngineUtils.h"

// ================= CONSTRUCTOR =================
AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetEnableGravity(false);

	// ================= HOLD POINT =================
	HoldPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HoldPoint"));
	HoldPoint->SetupAttachment(GetRootComponent());
	HoldPoint->SetRelativeLocation(FVector(200.f, 0.f, 60.f));

	// ================= MUZZLE =================
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GetMesh());
}

// ================= BEGIN PLAY =================
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	CurrentMaxSpeed = StartSpeed;

	GetWorldTimerManager().SetTimer(
		SpeedTimerHandle,
		this,
		&AMyPlayer::IncreaseSpeed,
		SpeedIncreaseInterval,
		true
	);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PlayerMappingContext)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(PlayerMappingContext, 0);
			}
		}
	}
}

// ================= SPEED =================
void AMyPlayer::IncreaseSpeed()
{
	CurrentMaxSpeed += SpeedIncreaseAmount;

	if (CurrentMaxSpeed > MaxSpeedLimit)
		CurrentMaxSpeed = MaxSpeedLimit;
}

// ================= TICK =================
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity = GetCapsuleComponent()->GetPhysicsLinearVelocity();
	Velocity.X = CurrentMaxSpeed;

	if (bIsFiring)
	{
		Velocity.Z += 200.f;
		if (Velocity.Z > 5000.f)
			Velocity.Z = 5000.f;
	}

	GetCapsuleComponent()->SetPhysicsLinearVelocity(Velocity);

	if (HeldObjects.Num() > 0)
	{
		UpdateHeldObject();
	}
}

// ================= INPUT =================
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input) return;

	if (JetpackAction)
	{
		Input->BindAction(JetpackAction, ETriggerEvent::Started, this, &AMyPlayer::JetpackStart);
		Input->BindAction(JetpackAction, ETriggerEvent::Completed, this, &AMyPlayer::JetpackStop);
	}

	if (FireAction)
	{
		Input->BindAction(FireAction, ETriggerEvent::Started, this, &AMyPlayer::StartFire);
		Input->BindAction(FireAction, ETriggerEvent::Completed, this, &AMyPlayer::StopFire);
	}

	if (GravityGunAction)
	{
		Input->BindAction(GravityGunAction, ETriggerEvent::Started, this, &AMyPlayer::GravityGunPressed);
		Input->BindAction(GravityGunAction, ETriggerEvent::Triggered, this, &AMyPlayer::GravityGunHeld);
		Input->BindAction(GravityGunAction, ETriggerEvent::Completed, this, &AMyPlayer::GravityGunReleased);
	}
}

// ================= JETPACK =================
void AMyPlayer::JetpackStart() { bIsFiring = true; }
void AMyPlayer::JetpackStop() { bIsFiring = false; }

// ================= FIRE =================
void AMyPlayer::StartFire()
{
	if (bIsShooting) return;

	bIsShooting = true;
	Fire();

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AMyPlayer::Fire, FireRate, true);
}

void AMyPlayer::StopFire()
{
	bIsShooting = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AMyPlayer::Fire()
{
	if (!bIsShooting || !ProjectileClass || !MuzzlePoint) return;

	GetWorld()->SpawnActor<AActor>(
		ProjectileClass,
		MuzzlePoint->GetComponentLocation(),
		GetControlRotation()
	);
}

void AMyPlayer::GravityGunReleased()
{
	// if grab state is active → do nothing
	if (bIsGrabed)
	{
		HoldTime = 0.f;
		bIsHoldingButton = true;
		return;
	}

	// nothing to throw
	if (HeldObjects.Num() <= 0)
		return;

	FVector PlayerVel = GetVelocity();
	float PlayerSpeed = PlayerVel.Size();
	FVector ForwardDir = GetActorForwardVector();

	FVector LaunchDir =
		(ForwardDir + PlayerVel.GetSafeNormal()).GetSafeNormal();

	for (AActor* Obj : HeldObjects)
	{
		if (!Obj) continue;

		if (UPrimitiveComponent* Prim =
			Cast<UPrimitiveComponent>(Obj->GetRootComponent()))
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Prim->SetSimulatePhysics(true);

			float Force =
				(PlayerSpeed * SpeedMultiplier) +
				ExtraForce +
				1000.f;

			Prim->AddImpulse(LaunchDir * Force, NAME_None, true);
		}
	}

	HeldObjects.Empty();
}
void AMyPlayer::GravityGunHeld()
{
	HoldTime += GetWorld()->GetDeltaSeconds();

	// optional delay before activation
	if (HoldTime < 0.3f)
		return;

	// if already grabbing, don't throw
	if (bIsGrabed && !bIsHoldingButton)
		return;

	// nothing to throw
	if (HeldObjects.Num() <= 0)
		return;

	FVector PlayerVel = GetVelocity();
	float PlayerSpeed = PlayerVel.Size();
	FVector ForwardDir = GetActorForwardVector();

	FVector LaunchDir =
		(ForwardDir + PlayerVel.GetSafeNormal()).GetSafeNormal();

	for (AActor* Obj : HeldObjects)
	{
		if (!Obj) continue;

		if (UPrimitiveComponent* Prim =
			Cast<UPrimitiveComponent>(Obj->GetRootComponent()))
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Prim->SetSimulatePhysics(true);

			float Force =
				(PlayerSpeed * SpeedMultiplier) +
				ExtraForce +
				1000.f;

			// ===================== LINEAR FORCE =====================
			Prim->AddImpulse(LaunchDir * Force, NAME_None, true);

			// ===================== RANDOM ANGULAR FORCE =====================

			// random rotation axis in ANY direction
			FVector RandomAxis = FMath::VRand().GetSafeNormal();

			// random spin strength
			float MinSpin = Force * 0.2f;
			float MaxSpin = Force * 1.5f;
			float RandomSpin = FMath::FRandRange(MinSpin, MaxSpin);

			// random flip direction (clockwise / counterclockwise)
			float Direction = FMath::RandBool() ? 1.f : -1.f;

			// FINAL SPIN
			Prim->AddAngularImpulseInDegrees(
				RandomAxis * RandomSpin * Direction,
				NAME_None,
				true
			);
		}
	}

	HeldObjects.Empty();
}
// ================= GRAVITY GUN =================
void AMyPlayer::GravityGunPressed()
{
	// holding object already = arm throw
	if (HeldObjects.Num() > 0)
	{
		bIsHoldingButton = false;
		HoldTime = 0.f;
		bIsGrabed = false;
		return;
	}

	// first grab
	bIsGrabed = true;

	AActor* Closest = nullptr;
	float BestDist = GrabRange;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* A = *It;
		if (!A || A == this) continue;

		for (TSubclassOf<AActor> Type : HoldableObjects)
		{
			if (!A->IsA(Type)) continue;

			float Dist = FVector::Dist(GetActorLocation(), A->GetActorLocation());

			if (Dist < BestDist)
			{
				BestDist = Dist;
				Closest = A;
			}
		}
	}

	if (Closest)
	{
		HeldObjects.Add(Closest);

		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Closest->GetRootComponent()))
		{
			Prim->SetSimulatePhysics(false);
			Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (AMyMissile* Missile = Cast<AMyMissile>(Closest))
		{
			Missile->IsHeld = true;
		}
	}
}

// ================= HOLD UPDATE =================
void AMyPlayer::UpdateHeldObject()
{
	if (!HoldPoint) return;

	FVector TargetPos = HoldPoint->GetComponentLocation();
	FRotator TargetRot = HoldPoint->GetComponentRotation();

	for (AActor* Obj : HeldObjects)
	{
		if (!Obj) continue;

		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Obj->GetRootComponent()))
		{
			Prim->SetWorldLocation(TargetPos);
			Prim->SetWorldRotation(TargetRot);
		}
	}
}