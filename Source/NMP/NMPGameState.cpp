// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPGameState.h"
#include "Net/UnrealNetwork.h"

/** Returns properties that are replicated for the lifetime of the actor channel */
void ANMPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANMPGameState, PlayState);
	DOREPLIFETIME(ANMPGameState, WinningPlayerName);
}