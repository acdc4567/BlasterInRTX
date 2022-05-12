// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

		virtual void Fire(const FVector& HitTarget) override;

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart,const FVector& HitTarget);

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);


private:
	UPROPERTY(EditAnywhere,Category=HitScan)
	float Damage = 10.f;


	UPROPERTY(EditAnywhere, Category = HitScan)
		class UParticleSystem* ImpactParticles;


	UPROPERTY(EditAnywhere, Category = HitScan)
		UParticleSystem* BeamParticles;


	//TraceEndWithScatter

	UPROPERTY(EditAnywhere, Category = WeaponScatter)
		float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = WeaponScatter)
		float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = WeaponScatter)
		bool bUseScatter = 0;



};
