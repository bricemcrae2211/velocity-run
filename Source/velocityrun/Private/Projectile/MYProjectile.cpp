#include "Projectile/MYProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// =========================
// CONSTRUCTOR
// =========================
AMYProjectile::AMYProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// =========================
	// COLLISION
	// =========================
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->InitSphereRadius(8.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Hit EVERYTHING
	Collision->SetCollisionResponseToAllChannels(ECR_Block);

	// NEVER collide with other projectiles
	Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	Collision->SetNotifyRigidBodyCollision(true);
	Collision->OnComponentHit.AddDynamic(this, &AMYProjectile::OnHit);

	// =========================
	// MESH
	// =========================
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// =========================
	// MOVEMENT
	// =========================
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	Movement->InitialSpeed = Speed;
	Movement->MaxSpeed = Speed;
	Movement->bRotationFollowsVelocity = true;
	Movement->ProjectileGravityScale = InitialGravityScale;

	// initial state
	CurrentGravityScale = InitialGravityScale;
	AliveTime = 0.f;
}

// =========================
// BEGIN PLAY
// =========================
void AMYProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Movement)
	{
		Movement->Velocity = GetActorForwardVector() * Speed;
	}

	SetLifeSpan(LifeTime);
}

// =========================
// TICK (BULLET PHYSICS)
// =========================
void AMYProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AliveTime += DeltaTime;

	// -------------------------
	// GRAVITY DELAY + RAMP
	// -------------------------
	if (AliveTime > GravityDelay && Movement)
	{
		CurrentGravityScale = FMath::FInterpTo(
			CurrentGravityScale,
			MaxGravityScale,
			DeltaTime,
			GravityRampSpeed
		);

		Movement->ProjectileGravityScale = CurrentGravityScale;
	}

	// -------------------------
	// DRAG (slow velocity over time)
	// -------------------------
	if (Movement)
	{
		FVector Vel = Movement->Velocity;
		Vel *= (1.f - Drag * DeltaTime);
		Movement->Velocity = Vel;
	}
}

// =========================
// HIT ANYTHING = DESTROY
// =========================
void AMYProjectile::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (!OtherActor || OtherActor == this)
		return;

	// NEVER collide with other projectiles
	if (OtherActor->IsA(AMYProjectile::StaticClass()))
		return;

	Destroy();
}