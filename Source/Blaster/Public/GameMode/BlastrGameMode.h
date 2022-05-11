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
	ABlastrGameMode();
	virtual void Tick(float DeltaTime) override;

	virtual void PlayerEliminated(class ABlasterCharacter* EliminatedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);

	
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);


	UPROPERTY(EditAnywhere)
		float WarmupTime = 10.f;

		float LevelStartingTime = 0.f;

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;




private:

		float CountdownTime = 0.f;




};
