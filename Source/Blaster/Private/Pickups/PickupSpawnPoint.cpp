// Blaster Game..All Rights Reserved!!!


#include "Pickups/PickupSpawnPoint.h"
#include "Pickups/Pickup.h"




// Sets default values
APickupSpawnPoint::APickupSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;




}

// Called when the game starts or when spawned
void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	StartSpawnPickupTimer((AActor*) nullptr);



}


// Called every frame
void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);






}



void APickupSpawnPoint::SpawnPickup() {

	int32 NumPickupClasses = PickupClasses.Num();
	if (NumPickupClasses>0) {
		int32 Selection = FMath::RandRange(0, NumPickupClasses - 1);
		SpawnedPickup= GetWorld()->SpawnActor<APickup>(PickupClasses[Selection],GetActorTransform());

		if (HasAuthority() && SpawnedPickup) {
			SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::StartSpawnPickupTimer);

		}


	}



}



void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor) {
	const float SpawnTime = FMath::RandRange(SpawnPickupTimeMin, SpawnPickupTimeMax);
	GetWorldTimerManager().SetTimer(SpawnPickupTimer, this, &APickupSpawnPoint::SpawnPickupTimerFinished, SpawnTime);



}


void APickupSpawnPoint::SpawnPickupTimerFinished() {

	if (HasAuthority()) {
		SpawnPickup();


	}

}





