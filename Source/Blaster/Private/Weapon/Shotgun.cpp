// Blaster Game..All Rights Reserved!!!


#include "Weapon/Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"



void AShotgun::Fire(const FVector& HitTarget) {

	AWeapon::Fire(HitTarget);
	

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)return;
	AController* InstigatorController = OwnerPawn->GetController();


	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket) {
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		//uint32 Hits = 0;
		TMap<ABlasterCharacter*, uint32> HitMap;
		
		for (uint32 i = 0; i < NumberOfPellets; i++) {
			//FVector End = TraceEndWithScatter(Start, HitTarget);
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && HasAuthority() && InstigatorController) {
				if (HitMap.Contains(BlasterCharacter)) {
					HitMap[BlasterCharacter]++;
				}
				else {
					HitMap.Emplace(BlasterCharacter, 1);
				}
				//UGameplayStatics::ApplyDamage(BlasterCharacter, Damage, InstigatorController, this, UDamageType::StaticClass());
				//++Hits;
			}
			if (ImpactParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
			}
			if (hitsound) {
				UGameplayStatics::PlaySoundAtLocation(this, hitsound, FireHit.ImpactPoint,.5f,FMath::FRandRange(-.5f,.5f));
			}




		}
		
		for (auto hitpair : HitMap) {
			if (hitpair.Key && HasAuthority() && InstigatorController) {

				UGameplayStatics::ApplyDamage(hitpair.Key, Damage*hitpair.Value, InstigatorController, this, UDamageType::StaticClass());

			}
		}


	}













}
