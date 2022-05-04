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
#include "PlayerController/BlasterPlayerController.h"





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
	DOREPLIFETIME(AWeapon, Ammo);




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

	SpendRound();





}

void AWeapon::Dropped() {


	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld,1);

	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;

}

void AWeapon::AddAmmo(int32 AmmoToAdd) {

	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);

	SetHUDAmmo();



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

void AWeapon::SetHUDAmmo() {

	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter) {
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if (BlasterOwnerController) {

			BlasterOwnerController->SetHUDWeaponAmmo(Ammo);


		}



	}







}

void AWeapon::SpendRound() {

	Ammo=FMath::Clamp(Ammo-1,0,MagCapacity);

	
	SetHUDAmmo();



}



void AWeapon::OnRep_Ammo() {

	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	
	
	
	SetHUDAmmo();

}


void AWeapon::OnRep_Owner() {

	Super::OnRep_Owner();
	if (Owner == nullptr) {
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else {
		SetHUDAmmo();
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
		WeaponMesh->SetSimulatePhysics(0);
		WeaponMesh->SetEnableGravity(0);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EWeaponState::EWS_Dropped:


		if (HasAuthority()) {
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(1);
		WeaponMesh->SetEnableGravity(1);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);




		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
	



}

bool AWeapon::IsEmpty() {

	return Ammo<=0;
}


void AWeapon::OnRep_WeaponState() {

	switch (WeaponState) {
	case EWeaponState::EWS_Initial:

		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(0);
		WeaponMesh->SetSimulatePhysics(0);
		WeaponMesh->SetEnableGravity(0);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EWeaponState::EWS_Dropped:

		WeaponMesh->SetSimulatePhysics(1);
		WeaponMesh->SetEnableGravity(1);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);



		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}



}





