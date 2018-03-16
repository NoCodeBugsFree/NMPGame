// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPHUD.h"
#include "Blueprint/UserWidget.h"
#include "ConstructorHelpers.h"

ANMPHUD::ANMPHUD()
{
	/**  set the widget asset */
	static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidgetClassBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/WBP_HUD"));
	if (HUDWidgetClassBPClass.Class != NULL)
	{
		HUDWidgetClass = HUDWidgetClassBPClass.Class;
	}
}

void ANMPHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateHUD();
}

void ANMPHUD::CreateHUD()
{
	if (HUDWidgetClass)
	{
		HUD_WidgetReference = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (HUD_WidgetReference)
		{
			HUD_WidgetReference->AddToViewport();
		}
	}
}