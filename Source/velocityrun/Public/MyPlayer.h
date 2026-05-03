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
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// ================= INPUT =================

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FlyAction;

	// ================= FLY =================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FlyForce = 50000.f;

	bool bFlying;

	void StartFly();
	void StopFly();
};