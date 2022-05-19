// Blaster Game..All Rights Reserved!!!


#include "Pickups/JumpPickup.h"
#include "Character/BlasterCharacter.h"
#include "BlasterComponents/BuffComponent.h"

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	if (BlasterCharacter) {
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff) {
			Buff->BuffJump(JumpZVelocityBuff, JumpBuffTime);
		}

	}
	Destroy();

}



