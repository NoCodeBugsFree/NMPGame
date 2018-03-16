// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NMPHUD.generated.h"

/**
 * 
 */
UCLASS()
class NMP_API ANMPHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	ANMPHUD();

	virtual void BeginPlay() override;

private:
	
	/** widget class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	
	/** current active widget */
	UPROPERTY()
	class UUserWidget* HUD_WidgetReference;

	/** calls to create HUD widget and add it to viewport  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CreateHUD();
	
};
