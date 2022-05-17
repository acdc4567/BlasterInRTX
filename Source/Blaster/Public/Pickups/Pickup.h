// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep ,const FHitResult& SweepResult);


	float BaseTurnRate = 45.f;


private:


	UPROPERTY(EditAnywhere, Category = Components)
		class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, Category = Components)
		class USoundCue* PickupSound;


	UPROPERTY(EditAnywhere, Category = Components)
		UStaticMeshComponent* PickupMesh;




public:	
	

};
