// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD_Widget.h"
#include "NMP_Player.h"
#include "NMPGameState.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

const static FText CollectPickupsText = FText::FromString("Collect Pickups !");
const static FText WTFText = FText::FromString("WTF ?");
const static FText GameOverText = FText::FromString("Game Over =(");

void UHUD_Widget::UpdateHUD()
{
	if (NMP_Player && PowerBar)
	{
		float InPercent = NMP_Player->GetPowerPercent();
		PowerBar->SetPercent(InPercent);
	}

	if (NMP_GameState && HelpText)
	{

		/** playing  */
		if (NMP_GameState->GetPlayState() == EPlayState::PS_Playing)
		{
			HelpText->SetText(CollectPickupsText);
		} 
		/** game over  */
		else if(NMP_GameState->GetPlayState() == EPlayState::PS_Lose)
		{
			HelpText->SetText(GameOverText);
		}
		/** win  */
		else if (NMP_GameState->GetPlayState() == EPlayState::PS_Win)
		{
			FString WinnerString = FString::Printf(TEXT("Player %s won the game !"), *NMP_GameState->GetWinningPlayerName());
			FText WinText = FText::FromString(WinnerString);
			HelpText->SetText(WinText);
		}
		/** unknown game state  */
		else
		{
			HelpText->SetText(WTFText);
		}
	}
}

void UHUD_Widget::NativeConstruct()
{
	/** set player reference  */
	if(ANMP_Player* TestNMP_Player = Cast<ANMP_Player>(GetOwningPlayerPawn()))
	{
		NMP_Player = TestNMP_Player;
	}

	/** set game state reference  */
	if(ANMPGameState* TestNMPGameState = GetWorld()->GetGameState<ANMPGameState>())
	{
		NMP_GameState = TestNMPGameState;
	}
}

void UHUD_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateHUD();
}
