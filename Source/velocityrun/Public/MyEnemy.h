#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnemy.generated.h"

UCLASS()
class VELOCITYRUN_API AMyEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AMyEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void FacePlayer();
	void UpdateMovement(float DeltaTime);

	UPROPERTY()
	APawn* Player;

	// ================= RANGE =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRange = 2000.f;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	bool bPlayerInRange = false;

	// ================= Z LEVEL (ADDED FIX) =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bPlayerSameLevel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ZTolerance = 150.f;

	// ================= SPEED =================
	UPROPERTY(EditAnywhere, Category = "AI")
	float DecayRate = 100.f;

	float CurrentSpeed = 0.f;
	bool bSpeedInitialized = false;
};