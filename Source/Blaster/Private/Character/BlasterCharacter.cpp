// Blaster Game..All Rights Reserved!!!


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "BlasterComponents/CombatComponent.h"
#include "BlasterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/BlasterAnimInstance.h"


ABlasterCharacter::ABlasterCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = 1;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = 0;

	bUseControllerRotationYaw = 0;
	GetCharacterMovement()->bOrientRotationToMovement = 1;


	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(RootComponent);


	Combat = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	Combat->SetIsReplicated(1);


	GetCharacterMovement()->NavAgentProps.bCanCrouch = 1;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCharacterMovement()->RotationRate = FRotator(0.f,0.f,850.f) ;


	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;




}



void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon,COND_OwnerOnly);




}

void ABlasterCharacter::PostInitializeComponents() {

	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character=this;
	}


}

void ABlasterCharacter::PlayFireMontage(bool bAiming) {
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage && FireWeaponMontagex1) {
		//UE_LOG(LogTemp, Warning, TEXT("Fire!!!"));
		if (bAiming) {
			AnimInstance->Montage_Play(FireWeaponMontagex1);
			AnimInstance->Montage_JumpToSection(FName("Default"));
		}
		else {
			AnimInstance->Montage_Play(FireWeaponMontage);
			AnimInstance->Montage_JumpToSection(FName("Default"));
		}



	}



}


void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABlasterCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);


}




void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);



	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABlasterCharacter::CrouchButtonReleased);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);





}




void ABlasterCharacter::MoveForward(float Value) {

	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	
	}

}



void ABlasterCharacter::MoveRight(float Value) {
	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);



	}


}

void ABlasterCharacter::Turn(float Value) {
	if (bIsAiming) {
		AddControllerYawInput(Value * AimTurnSens);
	}
	else {
		AddControllerYawInput(Value * HipTurnSens);
	}
	

}

void ABlasterCharacter::LookUp(float Value) {
	if (bIsAiming) {
		AddControllerPitchInput(Value * AimLookUpSens);
	}
	else {
		AddControllerPitchInput(Value * HipLookUpSens);
	}
	


}

void ABlasterCharacter::Jump() {
	if (bIsCrouched)
		UnCrouch();

	else
		Super::Jump();


}

void ABlasterCharacter::FireButtonPressed() {

	if (Combat) {
		Combat->FireButtonPressed(1);
	}



}

void ABlasterCharacter::FireButtonReleased() {

	if (Combat) {
		Combat->FireButtonPressed(0);
	}


}

void ABlasterCharacter::EquipButtonPressed() {
	if (Combat) {
		if (HasAuthority()) {
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else {
			ServerEquipButtonPressed();
		}
	}


}







void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(0);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled()) {
		if (OverlappingWeapon) {
			OverlappingWeapon->ShowPickupWidget(1);
		}
	}


}

bool ABlasterCharacter::IsWeaponEquipped() {

	return (Combat&&Combat->EquippedWeapon);
}


bool ABlasterCharacter::IsAiming() {

	return  (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() {


	if(Combat==nullptr)
	return nullptr;

	return Combat->EquippedWeapon;
}


void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) {

	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(1);
	}
	if (LastWeapon) {
		LastWeapon->ShowPickupWidget(0);
	}


}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation() {
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}




}

void ABlasterCharacter::CrouchButtonPressed() {
	Crouch();
}

void ABlasterCharacter::CrouchButtonReleased() {

	UnCrouch();
}

void ABlasterCharacter::AimButtonPressed() {
	bIsAiming = 1;
	if (Combat)
		Combat->SetAiming(1);

}

void ABlasterCharacter::AimButtonReleased() {
	bIsAiming = 0;
	if (Combat)
		Combat->SetAiming(0);

}

void ABlasterCharacter::AimOffset(float DeltaTime) {
	if (Combat && Combat->EquippedWeapon == nullptr)return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir) {
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
	
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning) {

			InterpAO_Yaw = AO_Yaw;


		}


		bUseControllerRotationYaw = 1;
		TurnInPlace(DeltaTime);
	
	}

	if (Speed > 0.f || bIsInAir) {

		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = 1;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;


	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
	
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	
	}

}


void ABlasterCharacter::TurnInPlace(float DeltaTime) {
	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw < -90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Left;

	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning) {
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f) {
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}


}