// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

class APickup;






UCLASS()
class BLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupSpawnPoint();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,Category=PickupSpawn)
	TArray<TSubclassOf< APickup>> PickupClasses;

	UPROPERTY()
		APickup* SpawnedPickup;


	void SpawnPickup();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);
	void SpawnPickupTimerFinished();
	
private:	


	FTimerHandle SpawnPickupTimer;

	UPROPERTY(EditAnywhere, Category = PickupSpawn)
		float SpawnPickupTimeMin = 4.f;


	UPROPERTY(EditAnywhere, Category = PickupSpawn)
		float SpawnPickupTimeMax = 14.f;


};
