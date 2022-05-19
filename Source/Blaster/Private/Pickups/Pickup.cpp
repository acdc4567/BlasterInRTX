// Blaster Game..All Rights Reserved!!!


#include "Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"



// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	
	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));


	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	PickupMesh->SetRenderCustomDepth(1);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);

	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>("PickupEffectComponent");
	PickupEffectComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);

	}
	


}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {







}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupMesh) {
		PickupMesh->AddLocalRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}

}

void APickup::Destroyed() {

	

	if (PickupSound) {
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	if (PickupEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation(), GetActorRotation());

	}
Super::Destroyed();

}

