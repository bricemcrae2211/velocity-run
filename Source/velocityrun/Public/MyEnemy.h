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

	// ================= FUNCTIONS =================
	void FacePlayer();
	void UpdateMovement(float DeltaTime);

	// ================= PLAYER =================
	UPROPERTY()
	APawn* Player;

	// ================= RANGE CHECK =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bPlayerInRange = false;

	// ================= Z LEVEL =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ZTolerance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bPlayerSameLevel = false;

	// ================= SPEED =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DecayRate = 100.f;

	float CurrentSpeed = 0.f;
	bool bSpeedInitialized = false;

	// ================= ADDED MOVEMENT TOGGLE =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bMoveForwardInsteadOfBackward = false;

	// ================= ADDED STATE FLAGS =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bHasFacedPlayer = false;

	// ================= NEW: FRONT CHECK =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bPlayerInFront = false;

	// distance forward check (how far in front player is)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float FrontDotThreshold = 0.0f; // 0 = strict front, -0.5 = wider cone
};