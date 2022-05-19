// Blaster Game..All Rights Reserved!!!


#include "BlasterComponents/BuffComponent.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




void UBuffComponent::Heal(float HealAmount, float HealingTime) {
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;




}

void UBuffComponent::HealRampUp(float DeltaTime) {

	if (!bHealing || Character == nullptr || Character->IsElimmed()) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth()) {
		bHealing = false;
		AmountToHeal = 0.f;
	}

}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed) {

	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;


}



// Called every frame
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp( DeltaTime);
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime) {

	if (Character == nullptr)return;
	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UBuffComponent::ResetSpeeds,BuffTime);

	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}


	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);


}


void UBuffComponent::ResetSpeeds() {

	if (Character == nullptr)return;
	if (Character->GetCharacterMovement()) {
		Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
		MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
	}


}

//_Implementation
void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed) {

	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;



}

