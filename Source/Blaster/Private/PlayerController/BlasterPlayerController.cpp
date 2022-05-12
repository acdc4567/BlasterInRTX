// Blaster Game..All Rights Reserved!!!


#include "PlayerController/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/Announcemnts.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/BlastrGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterComponents/CombatComponent.h"
#include "GameState/BlasterGameState.h"
#include "PlayerState/BlasterPlayerState.h"






void ABlasterPlayerController::BeginPlay() {

	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
	ServerCheckMatchState();

}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ABlasterPlayerController, MatchState);



}





void ABlasterPlayerController::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);
	SetHUDTime();

	CheckTimeSync(DeltaTime);
	PollInit();


}

float ABlasterPlayerController::GetServerTime() {

	if (HasAuthority())return GetWorld()->GetTimeSeconds();
	else {
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}

	
}

void ABlasterPlayerController::ReceivedPlayer() {
	Super::ReceivedPlayer();
	if (IsLocalController()) {
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	
	}

}

void ABlasterPlayerController::OnMatchStateSet(FName State) {

	MatchState = State;

	if (MatchState == MatchState::WaitingToStart) {

	}



	if (MatchState == MatchState::InProgress) {
		HandleMatchHasStarted();


	}
	else if (MatchState == MatchState::Cooldown) {
		HandleCooldown();
	}

}

void ABlasterPlayerController::OnRep_MatchState() {


	if (MatchState == MatchState::InProgress) {
		HandleMatchHasStarted();


	}

	else if (MatchState == MatchState::Cooldown) {
		HandleCooldown();
	}


}

void ABlasterPlayerController::HandleMatchHasStarted() {


	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD) {
		BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement) {

			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);


		}
	}

}


void ABlasterPlayerController::HandleCooldown() {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD) {
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = BlasterHUD->Announcement && BlasterHUD->Announcement->AnnouncementText && BlasterHUD->Announcement->InfoText;
		if (bHUDValid) {

			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In : ");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();

			if (BlasterGameState&& BlasterPlayerState) {

				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0) {
					InfoTextString = FString("There Is No Winner");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState) {
					InfoTextString = FString("You Are The Winner!!!");
				}
				else if (TopPlayers.Num() == 1) {
					InfoTextString = FString::Printf(TEXT("Winner : \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1) {
					InfoTextString = FString("Players Tied For The Win : \n");
					for (auto TiedPlayer : TopPlayers) {
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			
			

		}
	}

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter&&BlasterCharacter->GetCombat()) {
		BlasterCharacter->bDisableGameplay = 1;
		BlasterCharacter->GetCombat()->FireButtonPressed(0);


	}



}


void ABlasterPlayerController::SetHUDTime() {

	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState==MatchState::InProgress)TimeLeft=WarmupTime+MatchTime- GetServerTime() + LevelStartingTime;
	else if(MatchState==MatchState::Cooldown)TimeLeft =CooldownTime+ WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	


	uint32 SecondsLeft=FMath::CeilToInt(TimeLeft);

	if (HasAuthority()) {
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlastrGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode) {
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime()+LevelStartingTime);

		}
	
	}


	if (CountdownInt != SecondsLeft) {
		if (MatchState == MatchState::WaitingToStart|| MatchState == MatchState::Cooldown ) {
			SetHUDAnnouncementCountdown(TimeLeft);

		}
		if (MatchState == MatchState::InProgress) {


			SetHUDMatchCountdown(TimeLeft);
		}



	}

	CountdownInt = SecondsLeft;

	
}

void ABlasterPlayerController::PollInit() {
	if (CharacterOverlay == nullptr) {
		if (BlasterHUD && BlasterHUD->CharacterOverlay) {
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay) {
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			
			}
		
		
		}


	}




}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime) {

	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency) {
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}

}



void ABlasterPlayerController::ServerCheckMatchState_Implementation() {

	ABlastrGameMode* GameMode = Cast<ABlastrGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode){
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime = GameMode->CooldownTime;
		MatchState = GameMode->GetMatchState();
	
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime,LevelStartingTime);
		
	
	}




}




//_Implementation
void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch,float Warmup,float Match, float Cooldown,float StartingTime) {

	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (BlasterHUD && MatchState == MatchState::WaitingToStart) {
		BlasterHUD->AddAnnouncement();

	}

}



void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest) {

	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);


}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest) {

	float RoundTriptime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (.5f * RoundTriptime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();




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
	else {
		bInitializeCharacterOverlay = 1;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;

	}



}

void ABlasterPlayerController::SetHUDScore(float Score) {

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoredAmount;
	if (bHUDValid) {

		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));

		BlasterHUD->CharacterOverlay->ScoredAmount->SetText(FText::FromString(ScoreText));

	}
	else {
		bInitializeCharacterOverlay = 1;
		HUDScore = Score;
	}


}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats) {
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid) {

		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);

		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));

	}
	else {
		bInitializeCharacterOverlay = 1;
		HUDDefeats = Defeats;

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

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime) {


	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid) {
		if (CountdownTime < 0.f) {
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;

		}


		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;



		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));

	}




}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime) {

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->Announcement && BlasterHUD->Announcement->WarmupTime;
	if (bHUDValid) {

		if (CountdownTime < 0.f) {
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;

		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;



		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));

	}




}

void ABlasterPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter) {
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());



	}

}

