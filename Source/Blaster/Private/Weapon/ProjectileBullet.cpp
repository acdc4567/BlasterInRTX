// Blaster Game..All Rights Reserved!!!


#include "Weapon/ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBullet::AProjectileBullet() {
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->bRotationFollowsVelocity = 1;
	ProjectileMovementComponent->SetIsReplicated(1);
	ProjectileMovementComponent->InitialSpeed = 12000.f;
	ProjectileMovementComponent->MaxSpeed = 12000.f;
	ProjectileMovementComponent->ProjectileGravityScale = .02f;
}




void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {


	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter) {
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController) {


			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		
		
		}




	}




	Super::OnHit( HitComp,OtherActor, OtherComp,  NormalImpulse,  Hit);


}
