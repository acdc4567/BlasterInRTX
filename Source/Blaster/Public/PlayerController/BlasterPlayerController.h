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
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HandleMatchHasStarted();
	void HandleCooldown();

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
	
	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client,Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match,float Cooldown, float StartingTime);



private:

	UPROPERTY()
		class ABlasterHUD* BlasterHUD;

	UPROPERTY()
		class ABlastrGameMode* BlasterGameMode;



	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;

	float LevelStartingTime = 0.f;

	uint32 CountdownInt=0;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;


	UFUNCTION()
	void OnRep_MatchState();


	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay;

	//bool bInitializeCharacterOverlay = 0;

	float HUDHealth = 0.f;
	float HUDMaxHealth = 0.f;
	bool bInitializeHealth = 0;
	float HUDShield = 0.f;
	float HUDMaxShield = 0.f;
	bool bInitializeShield = 0;
	float HUDScore=0;
	bool bInitializeScore = 0;
	int32 HUDDefeats = 0;
	bool bInitializeDefeats = 0;
	int32 HUDCarriedAmmo = 0;
	bool bInitializeCarriedAmmo = 0;
	int32 HUDWeaponAmmo = 0;
	bool bInitializeWeaponAmmo = 0;

};
