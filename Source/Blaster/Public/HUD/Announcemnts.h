// Blaster Game..All Rights Reserved!!!

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcemnts.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UAnnouncemnts : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WarmupTime;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* AnnouncementText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* InfoText;










};
