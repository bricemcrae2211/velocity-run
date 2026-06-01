#include "GUI/MyPlayerWidget.h"
#include "Components/ProgressBar.h"

void UMyPlayerWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar)
		return;

	if (MaxHealth <= 0.f)
	{
		HealthBar->SetPercent(0.f);
		return;
	}

	float Percent = CurrentHealth / MaxHealth;

	Percent = FMath::Clamp(Percent, 0.f, 1.f);

	HealthBar->SetPercent(Percent);
}