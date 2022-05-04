// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"
#include "DrawDebugHelpers.h"
#include "Blaster/BlasterTypes/CombatState.h"



void UBlasterAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime) {
	Super::NativeUpdateAnimation(DeltaTime);


	if (BlasterCharacter == nullptr) {
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr)return;
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? 1 : 0;

	bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();

	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();

	bIsCrouched = BlasterCharacter->bIsCrouched;
	
	bIsAiming = BlasterCharacter->IsAiming();

	TurninInPlace = BlasterCharacter->GetTurningInPlace();

	bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();

	bElimmed = BlasterCharacter->IsElimmed();


	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
	YawOffset = DeltaRotation.Yaw;


	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target,DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);


	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	if (bIsWeaponEquipped&&EquippedWeapon&&EquippedWeapon->GetWeaponMesh()&&BlasterCharacter->GetMesh()) {
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"),ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled()) {
			FTransform RighHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RighHandTransform.GetLocation(), RighHandTransform.GetLocation() + (RighHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));

		}
		
		
		
	}

	bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;






}
