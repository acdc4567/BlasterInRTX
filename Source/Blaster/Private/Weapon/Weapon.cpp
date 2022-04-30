// Blaster Game..All Rights Reserved!!!


#include "Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon/Casing.h"
#include "Engine/SkeletalMeshSocket.h"





AWeapon::AWeapon()
{

	PrimaryActorTick.bCanEverTick = 0;
	bReplicates = 1;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(RootComponent);




}






void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (PickupWidget) {
		PickupWidget->SetVisibility(0);
	}

	if (HasAuthority()) {
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);

	
	}







}


void AWeapon::ShowPickupWidget(bool bShowWidget) {

	if (PickupWidget) {
		PickupWidget->SetVisibility(bShowWidget);
	}



}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);

}

void AWeapon::Fire(const FVector& HitTarget) {

	if (FireAnimation)WeaponMesh->PlayAnimation(FireAnimation, 0);

	if (CasingClass) {
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket) {
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());

			
			FActorSpawnParameters SpawnParams;
				
			UWorld* World = GetWorld();
			if (World) {
				World->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SpawnParams);


			}
			



		}

		

	}
}


void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter ) {
		BlasterCharacter->SetOverlappingWeapon(this);
	
	}



}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter) {
		BlasterCharacter->SetOverlappingWeapon(nullptr);

	}


}

void AWeapon::SetWeaponState(EWeaponState State) {

	WeaponState = State;
	switch (WeaponState) {
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:

		ShowPickupWidget(0);

		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
	



}


void AWeapon::OnRep_WeaponState() {

	switch (WeaponState) {
	case EWeaponState::EWS_Initial:

		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(0);
		
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}



}





