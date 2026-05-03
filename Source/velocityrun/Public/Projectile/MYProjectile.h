// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MYProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class VELOCITYRUN_API AMYProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMYProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// =========================
	// COMPONENTS
	// =========================
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement;

	// =========================
	// SETTINGS
	// =========================
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Speed = 6000.f; // faster machine gun feel

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LifeTime = 5.f;

	// =========================
	// BULLET BEHAVIOR CONTROL
	// =========================

	// how long before gravity fully kicks in
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Ballistics")
	float GravityDelay = 0.15f;

	// how strong gravity becomes over time
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Ballistics")
	float MaxGravityScale = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Ballistics")
	float InitialGravityScale = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Ballistics")
	float GravityRampSpeed = 2.5f;

	// slow velocity decay (air resistance feel)
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Ballistics")
	float Drag = 0.02f;

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

private:
	float CurrentGravityScale;
	float AliveTime;
};
