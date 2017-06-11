// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPGame.h"
#include "NMPGameGameState.h"

// [Server]
void ANMPGameGameState::SetCurrentState(EBatteryPlayState NewState)
{
	if (Role == ROLE_Authority)
	{
		CurrentState = NewState;
	}
}

void ANMPGameGameState::OnRep_CurrentState()
{
	// TODO
}

// GetLifetimeReplicatedProps
void ANMPGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANMPGameGameState, PowerToWin);
	DOREPLIFETIME(ANMPGameGameState, CurrentState);
	DOREPLIFETIME(ANMPGameGameState, WinningPlayerName);
}