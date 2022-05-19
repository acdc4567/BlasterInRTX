// Blaster Game..All Rights Reserved!!!


#include "Pickups/HealthPickup.h"
#include "Character/BlasterCharacter.h"
#include "BlasterComponents/BuffComponent.h"


AHealthPickup::AHealthPickup() {
	bReplicates = 1;
	


}



void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	Super::OnSphereOverlap( OverlappedComponent,OtherActor,OtherComp, OtherBodyIndex, bFromSweep,  SweepResult);

	ABlasterCharacter* BlasterCharacter= Cast<ABlasterCharacter>(OtherActor);
	
	if (BlasterCharacter) {
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff) {
			Buff->Heal(HealAmount, HealingTime);
		}

	}
	Destroy();

}
