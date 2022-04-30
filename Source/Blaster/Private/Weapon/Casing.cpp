// Blaster Game..All Rights Reserved!!!


#include "Weapon/Casing.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = 0;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>("CasingMesh");
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(1);
	CasingMesh->SetEnableGravity(1);
	CasingMesh->SetNotifyRigidBodyCollision(1);



}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this,&ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * 10.f);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	++PlaySoundCounter;
	DoPlaySoundOnce();
	SetLifeSpan(4.f);





}

void ACasing::DoPlaySoundOnce() {
	if (PlaySoundCounter < 2) {
		if (ShellSound)UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());

	}
	

}


