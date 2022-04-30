// Blaster Game..All Rights Reserved!!!


#include "GameMode/BlastrGameMode.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Character/BlasterCharacter.h"

void ABlastrGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) {

	if (EliminatedCharacter) {
		EliminatedCharacter->Elim();
	}
		





}
