// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"

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
	
	void PlayReloadMontage();


	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast,Reliable)
	void MulticastElim();

	virtual void Destroyed() override;
	UPROPERTY(Replicated)
	bool bDisableGameplay=0;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);






protected:
	

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void ReloadButtonPressed();
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
	
	//Poll for any relevent Classes and Initialize the HUD
	void PollInit();

	void RotateInPlace(float DeltaTime);


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

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ReloadMontage;


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

	FTimerHandle ElimTimer;

	void ElimTimerFinished();

	UPROPERTY(EditAnywhere, Category = PlayerStats)
		float ElimDelay = 5.f;

	//DissolveEffect

	UPROPERTY(VisibleAnywhere, Category = Dissolve)
		UTimelineComponent* DissolveTimeline;


	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere, Category = Dissolve)
		UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();


	UPROPERTY(VisibleAnywhere, Category = Dissolve)
		UMaterialInstanceDynamic* DynamicMaterialDissolveInstance;


	UPROPERTY(EditAnywhere, Category = Dissolve)
		UMaterialInstance* DissolveMaterialInstance;


	//ElimBot

	UPROPERTY(EditAnywhere, Category = Dissolve)
		UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere, Category = Dissolve)
		UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere, Category = Dissolve)
		class USoundCue* ElimBotSound;

	UPROPERTY()
		class ABlasterPlayerState* BlasterPlayerState;



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

	FORCEINLINE float GetHealth() const { return Health; }

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	ECombatState GetCombatState() const;

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }





};
