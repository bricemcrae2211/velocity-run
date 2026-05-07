#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyMissile.generated.h"

class UCapsuleComponent;
class AMyPlayer; // ?? forward declare player

UCLASS()
class VELOCITYRUN_API AMyMissile : public AActor
{
	GENERATED_BODY()

public:
	AMyMissile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ================= ROOT =================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* PhysicsBody;

	// ================= STATE =================
// ================= STATE =================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	bool IsHeld = false;

private:
	// ?? DIRECT PLAYER REFERENCE
	AMyPlayer* PlayerRef;

	UPROPERTY(EditAnywhere, Category = "Missile")
	float TriggerDistance = 4000.f;

	UPROPERTY(EditAnywhere, Category = "Missile")
	float LaunchDelay = 3.f;

	FTimerHandle LaunchTimer;

	bool bTriggered = false;
	bool bMovingBackward = false;
	bool bLaunched = false;

	void StartForwardLaunch();
	void PrintInRange();
};