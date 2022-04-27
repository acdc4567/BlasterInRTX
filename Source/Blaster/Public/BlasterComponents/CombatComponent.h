// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);


private:

	class ABlasterCharacter* Character;
	
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


public:	

	
		
};
