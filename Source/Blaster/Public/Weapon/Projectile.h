// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere, Category = Health)
		float Damage=10.f;

	UPROPERTY(VisibleAnywhere, Category = Components)
		class UProjectileMovementComponent* ProjectileMovementComponent;

private:

	UPROPERTY(EditAnywhere, Category=Components)
		class UBoxComponent* CollisionBox;


	
	UPROPERTY(EditAnywhere, Category = VFX)
		class UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComp;

	UPROPERTY(EditAnywhere, Category = VFX)
		UParticleSystem* ImpactParticles;
	
	
	UPROPERTY(EditAnywhere, Category = VFX)
		class USoundCue* ImpactSound;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
