#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerWidget.generated.h"

class UProgressBar;

UCLASS()
class VELOCITYRUN_API UMyPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float CurrentHealth, float MaxHealth);
};