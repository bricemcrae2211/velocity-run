#include "MyPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h" // ? ADD THIS
// ================= CONSTRUCTOR =================
AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetEnableGravity(false);

	// ?? Fire muzzle setup
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

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (PlayerMappingContext)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
					PlayerController->GetLocalPlayer()))
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
	{
		CurrentMaxSpeed = MaxSpeedLimit;
	}
}

// ================= TICK =================
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity = GetCapsuleComponent()->GetPhysicsLinearVelocity();

	// ================= FORWARD (NEVER CHANGED BY ANYTHING) =================
	Velocity.X = CurrentMaxSpeed;

	// ================= JETPACK (ONLY Z) =================
	if (bIsFiring)
	{
		Velocity.Z += 200.f;

		if (Velocity.Z > 5000.f)
		{
			Velocity.Z = 5000.f;
		}
	}

	// APPLY DIRECTLY
	GetCapsuleComponent()->SetPhysicsLinearVelocity(Velocity);
}

// ================= INPUT =================
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInput)
	{
		// JETPACK
		if (JetpackAction)
		{
			EnhancedInput->BindAction(JetpackAction, ETriggerEvent::Started, this, &AMyPlayer::JetpackStart);
			EnhancedInput->BindAction(JetpackAction, ETriggerEvent::Completed, this, &AMyPlayer::JetpackStop);
		}

		// FIRE (SEPARATE)
		if (FireAction)
		{
			EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &AMyPlayer::StartFire);
			EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, this, &AMyPlayer::StopFire);
		}
	}
}

// ================= JETPACK =================
void AMyPlayer::JetpackStart()
{
	bIsFiring = true;
	UE_LOG(LogTemp, Warning, TEXT("JETPACK ON"));
}

void AMyPlayer::JetpackStop()
{
	bIsFiring = false;
	UE_LOG(LogTemp, Warning, TEXT("JETPACK OFF"));
}

// ================= FIRE SYSTEM =================
void AMyPlayer::StartFire()
{
	if (bIsShooting) return;

	bIsShooting = true;

	UE_LOG(LogTemp, Warning, TEXT("FIRE START"));

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

	UE_LOG(LogTemp, Warning, TEXT("FIRE STOP"));

	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AMyPlayer::Fire()
{
	if (!bIsShooting) return;

	if (!ProjectileClass || !MuzzlePoint)
		return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
	FRotator SpawnRotation = GetControlRotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	World->SpawnActor<AActor>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);

	UE_LOG(LogTemp, Warning, TEXT("FIRE"));
}