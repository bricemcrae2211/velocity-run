#include "MyPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "MyMissile.h"

// ================= CONSTRUCTOR =================
AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetEnableGravity(false);

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

	// ================= GRAVITY GUN HOLD =================
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
	}
}

// ================= JETPACK =================
void AMyPlayer::JetpackStart()
{
	bIsFiring = true;
}

void AMyPlayer::JetpackStop()
{
	bIsFiring = false;
}

// ================= FIRE =================
void AMyPlayer::StartFire()
{
	if (bIsShooting) return;

	bIsShooting = true;
	Fire();

	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&AMyPlayer::Fire,
		FireRate,
		true
	);
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

// ================= GRAVITY GUN =================
void AMyPlayer::GravityGunPressed()
{
	// ================= RELEASE =================
	if (HeldObjects.Num() > 0)
	{
		FVector PlayerVel = GetVelocity();
		float PlayerSpeed = PlayerVel.Size();
		FVector ForwardDir = GetActorForwardVector();

		// one clean direction
		FVector LaunchDir = (ForwardDir + PlayerVel.GetSafeNormal()).GetSafeNormal();

		for (AActor* Obj : HeldObjects)
		{
			if (!Obj) continue;

			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Obj->GetRootComponent()))
			{
				Prim->SetSimulatePhysics(true);

				// ?? FORCE BASED ON PLAYER SPEED
				float TotalForce = (PlayerSpeed * SpeedMultiplier) + ExtraForce + 1000.f;

				FVector FinalImpulse = LaunchDir * TotalForce;

				// ignore mass ? strong throw
				Prim->AddImpulse(FinalImpulse, NAME_None, true);
			}
		}

		HeldObjects.Empty();
		return;
	}

	// ================= GRAB =================
	AActor* Closest = nullptr;
	float BestDist = GrabRange;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* A = *It;
		if (!A || A == this) continue;

		bool bIsHoldable = false;

		for (TSubclassOf<AActor> Type : HoldableObjects)
		{
			if (A->IsA(Type))
			{
				bIsHoldable = true;
				break;
			}
		}

		if (!bIsHoldable) continue;

		float Dist = FVector::Dist(GetActorLocation(), A->GetActorLocation());

		if (Dist < BestDist)
		{
			BestDist = Dist;
			Closest = A;
		}
	}

	if (Closest)
	{
		HeldObjects.Add(Closest);

		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Closest->GetRootComponent()))
		{
			Prim->SetSimulatePhysics(false);
		}

		// ?? mark missile as held (never reset)
		if (AMyMissile* Missile = Cast<AMyMissile>(Closest))
		{
			Missile->bIsHeld = true;
		}
	}
}

// ================= HOLD UPDATE =================
void AMyPlayer::UpdateHeldObject()
{
	if (HeldObjects.Num() == 0) return;

	FVector Pos = GetActorLocation() + GetActorForwardVector() * HoldDistance;

	for (AActor* Obj : HeldObjects)
	{
		if (!Obj) continue;

		Obj->SetActorLocation(Pos);
		Obj->SetActorRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f));
	}
}