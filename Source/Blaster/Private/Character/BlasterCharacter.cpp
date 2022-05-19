// Blaster Game..All Rights Reserved!!!


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "BlasterComponents/CombatComponent.h"
#include "BlasterComponents/BuffComponent.h"
#include "BlasterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/BlasterAnimInstance.h"
#include "Blaster/Blaster.h"
#include "PlayerController/BlasterPlayerController.h"
#include "GameMode/BlastrGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Weapon/WeaponTypes.h"



ABlasterCharacter::ABlasterCharacter()
{

	PrimaryActorTick.bCanEverTick = 1;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

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
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>("BuffComponent");
	Buff->SetIsReplicated(1);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = 1;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->RotationRate = FRotator(0.f,0.f,854.f) ;


	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>("DissolveTimelineComponent");



}

void ABlasterCharacter::BeginPlay() {
	Super::BeginPlay();

	Health = MaxHealth;
	Shield = MaxShield;
	UpdateHUDHealth();
	UpdateHUDShield();

	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

}


void ABlasterCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	RotateInPlace( DeltaTime);

	HideCameraIfCameraClose();
	PollInit();
}


void ABlasterCharacter::RotateInPlace(float DeltaTime) {
	if (bDisableGameplay) {
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
		
	
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled()) {
		AimOffset(DeltaTime);
	}
	else {
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > .25f) {
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}


}





void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon,COND_OwnerOnly);

	DOREPLIFETIME(ABlasterCharacter,Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);

	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);



}

void ABlasterCharacter::PostInitializeComponents() {

	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character=this;
	}
	if (Buff) {
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
	
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	
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

void ABlasterCharacter::PlayElimMontage() {

	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage) {
		
		AnimInstance->Montage_Play(ElimMontage);
		

	}





}

void ABlasterCharacter::PlayReloadMontage() {

	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage) {
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType()) {
		case EWeaponType::EWT_AssaultRifle:

			SectionName = FName("Rifle");

			break;
		case EWeaponType::EWT_RocketLauncher:

			SectionName = FName("Rifle");

			break;
		case EWeaponType::EWT_Pistol:

			SectionName = FName("Rifle");

			break;
		case EWeaponType::EWT_Shotgun:

			SectionName = FName("Rifle");

			break;
		case EWeaponType::EWT_SniperRifle:

			SectionName = FName("Rifle");

			break;
		case EWeaponType::EWT_GrenadeLauncher:

			SectionName = FName("Rifle");

			break;
		case EWeaponType::EWT_MAX:
			break;
		default:
			break;
		}



		AnimInstance->Montage_Play(ReloadMontage);
		AnimInstance->Montage_JumpToSection(SectionName);


	}







}

void ABlasterCharacter::PlayHitReactMontage() {

	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage) {
		FName SectionName("FromLeft");
		
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
		

	}


}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {

	if (bElimmed)return;
	float DamageToHealth = Damage;
	if (Shield > 0) {
		if (Shield >= Damage) {
			Shield = FMath::Clamp(Shield - Damage, 0, MaxShield);
			DamageToHealth = 0;
		}
		else {
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0, Damage);
			Shield = 0.f;

		}
	}


	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();
	if (Health == 0.f) {
		ABlastrGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlastrGameMode>();
		if (BlasterGameMode) {
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatedBy);
			BlasterGameMode->PlayerEliminated(this,BlasterPlayerController, AttackerController);
		}
	}
	
}

void ABlasterCharacter::UpdateHUDHealth() {

	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;

	if (BlasterPlayerController) {
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDShield() {
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;

	if (BlasterPlayerController) {
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}


}

void ABlasterCharacter::PollInit() {

	if (BlasterPlayerState == nullptr) {
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState) {
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);




		}
		


	}


}



void ABlasterCharacter::OnRep_ReplicatedMovement() {

	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	
	TimeSinceLastMovementReplication = 0.f;






}

void ABlasterCharacter::Elim() {

	if (Combat && Combat->EquippedWeapon) {
		Combat->EquippedWeapon->Dropped();
	}


	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer,this,&ABlasterCharacter::ElimTimerFinished,ElimDelay);

	



}

void ABlasterCharacter::Destroyed() {
	Super::Destroyed();

	if (ElimBotComponent) {
		ElimBotComponent->DestroyComponent();
	}

	ABlastrGameMode* BlasterGameMode = Cast<ABlastrGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;


	if (Combat && Combat->EquippedWeapon&& bMatchNotInProgress) {
		Combat->EquippedWeapon->Destroy();
	}


}

void ABlasterCharacter::MulticastElim_Implementation() {
	
	
	if (BlasterPlayerController)BlasterPlayerController->SetHUDWeaponAmmo(0);
	
	
	bElimmed = 1;
	PlayElimMontage();

	//StartDissolveEffect

	if (DissolveMaterialInstance) {
		DynamicMaterialDissolveInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMaterialDissolveInstance);

		DynamicMaterialDissolveInstance->SetScalarParameterValue(TEXT("Dissolve"), .0f);
		DynamicMaterialDissolveInstance->SetScalarParameterValue(TEXT("Glow"), 100.0f);



	}
	StartDissolve();

	//DisableCharacterMovement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = 1;
	if (Combat) {
		Combat->FireButtonPressed(0);
	}
	//DisableCollision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//SpawnElimBot

	if (ElimBotEffect) {
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimBotEffect, ElimBotSpawnPoint, GetActorRotation());

	}

	if (ElimBotSound) {
		UGameplayStatics::SpawnSoundAtLocation(this, ElimBotSound, GetActorLocation());
	}

	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope) {
		ShowSniperScopeWidget(0);
	}


}


void ABlasterCharacter::ElimTimerFinished() {
	



	ABlastrGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlastrGameMode>();
	if (BlasterGameMode) {
	
		BlasterGameMode->RequestRespawn(this,Controller);
	
	}
	








}






void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jumpx", IE_Pressed, this, &ABlasterCharacter::Jump);



	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);

	PlayerInputComponent->BindAction("Equipx", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABlasterCharacter::CrouchButtonReleased);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPressed);






}




void ABlasterCharacter::MoveForward(float Value) {
	if (bDisableGameplay)return;

	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	
	}

}



void ABlasterCharacter::MoveRight(float Value) {
	if (bDisableGameplay)return;

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
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->FireButtonPressed(1);
	}



}

void ABlasterCharacter::FireButtonReleased() {
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->FireButtonPressed(0);
	}


}

void ABlasterCharacter::EquipButtonPressed() {
	if (bDisableGameplay)return;

	if (Combat) {
		if (HasAuthority()) {
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else {
			ServerEquipButtonPressed();
		}
	}


}







void ABlasterCharacter::HideCameraIfCameraClose() {

	if (!IsLocallyControlled())return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold) {
		GetMesh()->SetVisibility(0);
		if (Combat && Combat->EquippedWeapon&&Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = 1;

		}


	}
	else {
		GetMesh()->SetVisibility(1);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = 0;

		}

	}



}



float ABlasterCharacter::CalculateSpeed() {

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	


	return Velocity.Size();
}

void ABlasterCharacter::OnRep_Health(float LastHealth) {
	UpdateHUDHealth();
	if (Health<LastHealth) {
		PlayHitReactMontage();
	}
	

}

void ABlasterCharacter::OnRep_Shield(float LastShield) {
	UpdateHUDShield();
	if (Shield < LastShield) {
		PlayHitReactMontage();
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

FVector ABlasterCharacter::GetHitTarget() const {




	if(Combat==nullptr)return FVector();
	return Combat->HitTarget;
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue) {

	if (DynamicMaterialDissolveInstance) {
		DynamicMaterialDissolveInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}



}

void ABlasterCharacter::StartDissolve() {

	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve&&DissolveTimeline) {
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}


}

ECombatState ABlasterCharacter::GetCombatState() const {


	if (Combat == nullptr)return ECombatState::ECS_MAX;
	return Combat->CombatState;
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
	if (bDisableGameplay)return;


	Crouch();
}

void ABlasterCharacter::CrouchButtonReleased() {
	if (bDisableGameplay)return;

	UnCrouch();
}

void ABlasterCharacter::ReloadButtonPressed() {
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->Reload();
	}









}

void ABlasterCharacter::AimButtonPressed() {
	if (bDisableGameplay)return;


	bIsAiming = 1;
	if (Combat)
		Combat->SetAiming(1);

}

void ABlasterCharacter::AimButtonReleased() {
	if (bDisableGameplay)return;

	bIsAiming = 0;
	if (Combat)
		Combat->SetAiming(0);

}

void ABlasterCharacter::AimOffset(float DeltaTime) {
	if (Combat && Combat->EquippedWeapon == nullptr)return;
	
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir) {
		bRotateRootBone = 1;

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
		bRotateRootBone = 0;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = 1;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;


	}

	CalculateAO_Pitch();

}

void ABlasterCharacter::CalculateAO_Pitch() {
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);

		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);

	}
}

void ABlasterCharacter::SimProxiesTurn() {

	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	bRotateRootBone = 0;
	float Speed = CalculateSpeed();
	if (Speed > 0.f) {
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw=(UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame)).Yaw;
	if (FMath::Abs(ProxyYaw) > TurnThreshold) {

		if (ProxyYaw > TurnThreshold) {
			TurningInPlace = ETurningInPlace::ETIP_Right;

		}
		else if (ProxyYaw < -TurnThreshold) {


			TurningInPlace = ETurningInPlace::ETIP_Left;

		}
		else {
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;

		}
		return;
	}

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;




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