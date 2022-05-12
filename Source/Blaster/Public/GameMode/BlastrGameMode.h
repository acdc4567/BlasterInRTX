// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlastrGameMode.generated.h"


namespace MatchState {
	extern BLASTER_API const FName Cooldown;
}




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

	UPROPERTY(EditDefaultsOnly)
		float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
		float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
		float CooldownTime = 10.f;

	
		float LevelStartingTime = 0.f;

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	

private:

		float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime()const { return CountdownTime; }


};
