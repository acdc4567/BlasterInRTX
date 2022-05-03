// Blaster Game..All Rights Reserved!!!


#include "PlayerController/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/BlasterCharacter.h"





void ABlasterPlayerController::BeginPlay() {

	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());


}






void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HealthBar && BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid) {
		

		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));

	}



}

void ABlasterPlayerController::SetHUDScore(float Score) {

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoredAmount;
	if (bHUDValid) {

		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));

		BlasterHUD->CharacterOverlay->ScoredAmount->SetText(FText::FromString(ScoreText));

	}


}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid) {

		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);

		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));

	}





}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo) {

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->AmmoAmount;
	if (bHUDValid) {

		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);

		BlasterHUD->CharacterOverlay->AmmoAmount->SetText(FText::FromString(AmmoText));

	}



}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo) {


	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid) {

		FString CarriedAmmoText = FString::Printf(TEXT("%d"), Ammo);

		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));

	}







}

void ABlasterPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter) {
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());



	}

}
