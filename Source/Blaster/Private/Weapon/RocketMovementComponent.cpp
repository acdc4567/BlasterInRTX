// Blaster Game..All Rights Reserved!!!


#include "Weapon/RocketMovementComponent.h"

URocketMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) {
	
	
	Super::HandleBlockingHit(Hit,  TimeTick,  MoveDelta,  SubTickTimeRemaining);

	return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) {
	//Super::HandleImpact( Hit,  TimeSlice,  MoveDelta)

}
