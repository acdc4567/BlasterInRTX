// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter,public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	

	ABlasterCharacter();


	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PostInitializeComponents() override;

	void PlayFireMontage(bool bAiming);

	void PlayElimMontage();
	

	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(NetMulticast,Reliable)
	void Elim();



protected:
	

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();


	UPROPERTY(EditAnywhere, Category = MouseInput)
		float HipTurnSens = .7f;
	UPROPERTY(EditAnywhere, Category = MouseInput)
		float HipLookUpSens = .7f;
	UPROPERTY(EditAnywhere, Category = MouseInput)
		float AimTurnSens = .5f;
	UPROPERTY(EditAnywhere, Category = MouseInput)
		float AimLookUpSens = .5f;

	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	void UpdateHUDHealth();
	





private:
	UPROPERTY(VisibleAnywhere,Category=Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Widget,meta=(AllowPrivateAccess="true"))
		class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing= OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* Combat;

	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* FireWeaponMontagex1;

	void HideCameraIfCameraClose();

	UPROPERTY(EditAnywhere, Category = Combat)
		float CameraThreshold = 200.f;

	bool bRotateRootBone;
	UPROPERTY(EditAnywhere, Category = Combat)
		float TurnThreshold = .5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	//PlayerHealth

	UPROPERTY(EditAnywhere, Category = PlayerStats)
		float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere,Category = PlayerStats)
		float Health;

	UFUNCTION()
		void OnRep_Health();

	class ABlasterPlayerController* BlasterPlayerController;


	bool bElimmed = 0;


public:	
	
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped();
	bool IsAiming();
	bool bIsAiming=0;

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();


	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }

	FORCEINLINE bool IsElimmed() const { return bElimmed; }





};
