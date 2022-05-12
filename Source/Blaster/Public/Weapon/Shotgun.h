// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()
	

public:

	virtual void Fire(const FVector& HitTarget) override;


private:
	UPROPERTY(EditAnywhere,Category =WeaponScatter)
	uint32 NumberOfPellets = 12;




};
