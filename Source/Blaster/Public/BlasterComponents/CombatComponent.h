// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend class ABlasterCharacter;

	void EquipWeapon(class AWeapon* WeaponToEquip);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();


	void FireButtonPressed(bool bPressed);

	void Fire();

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);


	void SetHUDPackage(float DeltaTime);


private:

	class ABlasterCharacter* Character;
	class ABlasterPlayerController* BlasterController;
	class ABlasterHUD* HUD;



	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WalkSpeed, meta = (AllowPrivateAccess = "true"))
		float BaseWalkSpeed = 600.f;;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WalkSpeed, meta = (AllowPrivateAccess = "true"))
		float AimWalkSpeed=450.f;

	bool bFireButtonPressed=0;

	FVector HitTarget;


	//HUD and Crosshairs

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FHUDPackage HUDPackage;
	//Aiming And FOV

	UPROPERTY(EditAnywhere,Category=Combat)
		float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
		float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomInterpSpeed = 20.f;
	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomedFOV = 30.f;

	void InterpFOV(float DeltaTime);

	//AutomaticFire
	FTimerHandle FireTimer;
	
	
bool bCanFire=1;
	void StartFireTimer();
	void FireTimerFinished();




public:	

	
		
};
