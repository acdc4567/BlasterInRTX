// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponTypes.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState :uint8 {
	
	EWS_Initial UMETA(DisplayName="Initial State")
	,EWS_Equipped UMETA(DisplayName="Equipped")
	, EWS_Dropped UMETA(DisplayName = "Dropped")

	, EWS_MAX UMETA(DisplayName = "DefaultMAX")

};





UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:

	AWeapon();
	
	void ShowPickupWidget(bool bShowWidget);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	virtual void Fire(const FVector & HitTarget);
	
	void Dropped();

	void AddAmmo(int32 AmmoToAdd);



	//AutomaticFire

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = 1;

	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere)
		class USoundCue* EquipSound;

	//enable or disable custom Depth

	void EnableCustomDepth(bool bEnable);





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState WeaponState;

	UFUNCTION()
		void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* PickupWidget;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Animation", meta = (AllowPrivateAccess = "true"))
		class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACasing> CasingClass;

	
	//Zoomed FOV While Aiming
	UPROPERTY(EditAnywhere)
		float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed = 20.f;


	UPROPERTY(EditAnywhere, ReplicatedUsing= OnRep_Ammo,Category = Ammo)
		int32 Ammo;

	UFUNCTION()
		void OnRep_Ammo();

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 MagCapacity;

	void SpendRound();


	UPROPERTY()
		class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
		class ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditAnywhere, Category = Ammo)
		EWeaponType WeaponType;





public:	
	void SetWeaponState(EWeaponState State);

	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere; }

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	//textures for weapon crosshairs

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	bool IsEmpty();


	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }


	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }


};
