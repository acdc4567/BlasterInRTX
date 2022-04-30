// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);



private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
		class USoundCue* ShellSound;

	int32 PlaySoundCounter = 0;

	void DoPlaySoundOnce();
};
