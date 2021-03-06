// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/BlasterHUD.h"
#include "Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"

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
	void SwapWeapons();

	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	void FireButtonPressed(bool bPressed);


	void PickupAmmo(EWeaponType WeaponType,int32 AmmoAmount);


	bool ShowldSwapWeapons();


protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
		void OnRep_SecondaryWeapon();

	
	void Fire();

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);


	void SetHUDPackage(float DeltaTime);


	UFUNCTION(Server, Reliable)
		void ServerReload();


	void HandleReload();

	int32 AmountToReload();


	void UpdateCarriedAmmo();

	void DropEquippedWeapon();

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);





private:

	UPROPERTY()
		class ABlasterCharacter* Character;
	UPROPERTY()
		class ABlasterPlayerController* BlasterController;
	UPROPERTY()
		class ABlasterHUD* HUD;



	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
		AWeapon* SecondaryWeapon;

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

	bool CanFire();
	

	//CarriedAmmoForTheCurrentlyEquippedWeapon
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
		int32 CarriedAmmo;

	UFUNCTION()
		void OnRep_CarriedAmmo();


	TMap<EWeaponType, int32>CarriedAmmoMap;
	
	
	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 MaxCarriedAmmo = 500;


	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 StartingARAmmo = 130;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 StartingRocketAmmo = 10;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 StartingPistolAmmo = 50;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 StartingShotgunAmmo = 15;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 StartingSniperAmmo = 15;
	
	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 StartingGrenadeLauncherAmmo = 15;


	void InitializeCarriedAmmo();


	UPROPERTY(ReplicatedUsing= OnRep_CombatState)
		ECombatState CombatState=ECombatState::ECS_Unoccupied;

	UFUNCTION()
		void OnRep_CombatState();


	void UpdateAmmoValues();





public:	

	
		
};
