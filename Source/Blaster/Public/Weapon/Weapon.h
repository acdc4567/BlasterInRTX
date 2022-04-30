// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	virtual void Fire(const FVector & HitTarget);
	
	//AutomaticFire

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = 1;

	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = 0.15f;


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


};
