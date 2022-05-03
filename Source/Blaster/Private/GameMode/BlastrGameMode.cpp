// Blaster Game..All Rights Reserved!!!


#include "GameMode/BlastrGameMode.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerState/BlasterPlayerState.h"




void ABlastrGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) {


	ABlasterPlayerState* AttackerPlayerState = AttackerController  ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState) {
		AttackerPlayerState->AddToScore(1.f);

	}
	if (VictimPlayerState)VictimPlayerState->AddToDefeats(1);

	if (EliminatedCharacter) {
		EliminatedCharacter->Elim();
	}
		





}

void ABlastrGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController) {


	if (ElimmedCharacter) {
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController) {

		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);


		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);

	}



}
