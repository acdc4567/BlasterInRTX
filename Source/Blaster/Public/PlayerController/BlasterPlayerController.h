// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDHealth(float Health,float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HandleMatchHasStarted();

protected:
	virtual void BeginPlay() override;

	void SetHUDTime();
	void PollInit();


	//Sync Time BW Client n Server

	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);


	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = Time)
		float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	

private:

	UPROPERTY()
		class ABlasterHUD* BlasterHUD;

	float MatchTime = 120.f;
	uint32 CountdownInt=0;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;


	UFUNCTION()
	void OnRep_MatchState();


	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = 0;

	float HUDHealth = 0.f;
	float HUDMaxHealth = 0.f;
	float HUDScore=0;
	int32 HUDDefeats = 0;



};
