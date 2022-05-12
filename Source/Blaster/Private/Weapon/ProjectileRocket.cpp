// Blaster Game..All Rights Reserved!!!


#include "Weapon/ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/RocketMovementComponent.h"

AProjectileRocket::AProjectileRocket() {

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>("RocketMovement");
	RocketMovementComponent->bRotationFollowsVelocity = 1;
	RocketMovementComponent->SetIsReplicated(1);


}








void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn) {
		AController* FiringController = FiringPawn->GetController();
		if (FiringController) {
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 10.f, GetActorLocation(), 200.f, 500.f, 1.f, UDamageType::StaticClass(), TArray<AActor* >(), this,FiringController);


		}


	}
	
	
	
	
	Super::OnHit( HitComp, OtherActor,OtherComp,  NormalImpulse, Hit);
}
