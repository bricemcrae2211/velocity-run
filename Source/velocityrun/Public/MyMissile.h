#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyMissile.generated.h"

class UCapsuleComponent;

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

	// ================= ROOT / PHYSICS =================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* PhysicsBody;

	// ================= STATE =================
	UPROPERTY(EditAnywhere, Category = "Missile")
	bool IsHeld = false;

private:
	AActor* PlayerActor;

	UPROPERTY(EditAnywhere, Category = "Missile")
	float TriggerDistance = 4000.f;

	UPROPERTY(EditAnywhere, Category = "Missile")
	float LaunchDelay = 3.f;

	UPROPERTY(EditAnywhere, Category = "Missile")
	float BackwardSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Missile")
	float ForwardSpeed = 2000.f;

	FTimerHandle LaunchTimer;

	bool bTriggered = false;
	bool bMovingBackward = false;
	bool bLaunched = false;

	void StartForwardLaunch();
	void PrintInRange();
};