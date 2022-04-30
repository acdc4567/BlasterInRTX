// Blaster Game..All Rights Reserved!!!


#include "BlasterComponents/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"





UCombatComponent::UCombatComponent()
{
	

	PrimaryComponentTick.bCanEverTick = 1;

	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (Character->GetFollowCamera()) {
		
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
	
	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

	if (Character && Character->IsLocallyControlled()) {
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		SetHUDPackage(DeltaTime);
		HitTarget = HitResult.ImpactPoint;

		InterpFOV(DeltaTime);
	}
	


}





void UCombatComponent::FireButtonPressed(bool bPressed) {

	bFireButtonPressed = bPressed;
	
	if (bFireButtonPressed&&EquippedWeapon) {
		
		Fire();

	}
	

}

void UCombatComponent::Fire() {
	
	if (bCanFire) {
		bCanFire = 0;
		ServerFire(HitTarget);

		if (EquippedWeapon) {
			CrosshairShootingFactor = .55f;
		}
		StartFireTimer();
	}
	
}

void UCombatComponent::StartFireTimer() {

	if (EquippedWeapon == nullptr || Character == nullptr)return;

	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);



}

void UCombatComponent::FireTimerFinished() {

	if (EquippedWeapon == nullptr)return;

	bCanFire = 1;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic) {
		Fire();
	}



}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget) {

	MulticastFire(TraceHitTarget);

}


void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget) {

	if (EquippedWeapon == nullptr)return;
	if (Character) {
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}


}



void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip) {


	if (Character == nullptr||WeaponToEquip==nullptr)return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket= Character->GetMesh()->GetSocketByName(TEXT("RightHandSocket"));
	if (HandSocket) {
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	
	
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = 0;
	Character->bUseControllerRotationYaw = 1;


}

void UCombatComponent::OnRep_EquippedWeapon() {
	if (EquippedWeapon && Character) {
		Character->GetCharacterMovement()->bOrientRotationToMovement = 0;
		Character->bUseControllerRotationYaw = 1;
	}


}


void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult) {


	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld= UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);


	if (bScreenToWorld) {
		FVector Start = CrosshairWorldPosition;

		if (Character) {
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.0f);


		}




		FVector End = Start + CrosshairWorldDirection * 80000.f;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
	
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>()) {
			HUDPackage.CrosshairsColor = FLinearColor::Red;

		}
		else {
			HUDPackage.CrosshairsColor = FLinearColor(1.0f, .94f, .22f, 0.75f);

		}
		

	
	}



}

void UCombatComponent::SetHUDPackage(float DeltaTime) {


	if (Character == nullptr || Character->Controller == nullptr)return;
	BlasterController = BlasterController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : BlasterController;
	if (BlasterController) {
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(BlasterController->GetHUD()) : HUD;
		if (HUD) {

			if (EquippedWeapon) {

				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else {

				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;

			}

			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling()) CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			else CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 32.25f);

			if (bAiming)CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, .58f, DeltaTime, 30.f);
			else CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.f);

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 30.f);


			HUDPackage.CrosshairSpread = .5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;




			HUD->SetHUDPackage(HUDPackage);

		}


	}




}

void UCombatComponent::InterpFOV(float DeltaTime) {

	if (EquippedWeapon == nullptr) return;
	if (bAiming) {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera()) {
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}






}



void UCombatComponent::SetAiming(bool bIsAiming) {
	bAiming = bIsAiming;

	ServerSetAiming(bIsAiming);
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}


}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming) {
	bAiming = bIsAiming;
	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}

}
