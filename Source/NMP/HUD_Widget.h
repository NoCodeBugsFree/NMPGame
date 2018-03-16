// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD_Widget.generated.h"

/**
 * 
 */
UCLASS()
class NMP_API UHUD_Widget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	
	/** call in NativeTick function to update HUD to valid state  */
	void UpdateHUD();

	/** calls when widget is constructed  */
	virtual void NativeConstruct() override;

	/** call every frame  */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// -----------------------------------------------------------------------------------

	/**  HelpText reference  */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HelpText;

	/**  PowerBar reference  */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PowerBar;

	/** owning player reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ANMP_Player* NMP_Player;

	/** game state reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ANMPGameState* NMP_GameState;
	
};
