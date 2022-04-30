// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlastrGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlastrGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	virtual void PlayerEliminated(class ABlasterCharacter* EliminatedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);




};
