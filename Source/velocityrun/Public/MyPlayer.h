#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyPlayer.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class VELOCITYRUN_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AMyPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ================= SPEED =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float StartSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float MaxSpeedLimit = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float SpeedIncreaseAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float SpeedIncreaseInterval = 2.0f;

	float CurrentMaxSpeed;
	FTimerHandle SpeedTimerHandle;

	void IncreaseSpeed();

	// ================= INPUT =================
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JetpackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	// ================= JETPACK =================
	bool bIsFiring = false;

	void JetpackStart();
	void JetpackStop();

	// ================= FIRE =================
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Fire")
	USceneComponent* MuzzlePoint;

	FTimerHandle FireTimerHandle;
	float FireRate = 0.15f;

	bool bIsShooting = false;

	void StartFire();
	void StopFire();
	void Fire();

	// ================= GRAVITY GUN =================
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* GravityGunAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	TArray<TSubclassOf<AActor>> HoldableObjects;
	UPROPERTY()
	TArray<AActor*> HeldObjects;

	UPROPERTY(EditAnywhere, Category = "GravityGun")
	float GrabRange = 300.f;

	UPROPERTY(EditAnywhere, Category = "GravityGun")
	float HoldDistance = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float SpeedMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float ExtraForce = 500.f;
	bool bHolding = false;

	void GravityGunPressed();
	void UpdateHeldObject();
};