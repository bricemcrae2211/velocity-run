#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyPlayer.generated.h"

class UInputMappingContext;
class UInputAction;
class USceneComponent;

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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* GravityGunAction;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	TArray<TSubclassOf<AActor>> HoldableObjects;

	// ================= CHARGE SYSTEM =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float MaxChargeTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float MinForce = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float MaxForce = 6000.f;

	float ChargeTime = 0.f;
	bool bIsCharging = false;
	bool bIsGrabed = false;

	UPROPERTY()
	TArray<AActor*> HeldObjects;

	UPROPERTY(EditAnywhere, Category = "GravityGun")
	float GrabRange = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float SpeedMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityGun")
	float ExtraForce = 500.f;

	float HoldTime = 0.0f;

	// HOLD POINT
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GravityGun")
	USceneComponent* HoldPoint;

	FTimerHandle AutoReleaseHandle;
	bool bIsHoldingButton = false;

	// ================= GRAVITY GUN INPUT =================
	UFUNCTION()
	void GravityGunPressed();

	UFUNCTION()
	void GravityGunHeld();

	UFUNCTION()
	void GravityGunReleased();

	void UpdateHeldObject();
};