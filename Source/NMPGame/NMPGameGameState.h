// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "NMPGameGameState.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBatteryPlayState : uint8
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};


UCLASS()
class NMPGAME_API ANMPGameGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	/** set by GameMode  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AAA")
	float PowerToWin;

	UFUNCTION(BlueprintPure, Category = "AAA")
	EBatteryPlayState GetCurrentState() const { return CurrentState; }
	
	// Server only
	void SetCurrentState(EBatteryPlayState NewState);

	// RepNotify fired on clients to allow client's changes
	UFUNCTION()
	virtual void OnRep_CurrentState();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "AAA")
	FString WinningPlayerName;
	
private:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	EBatteryPlayState CurrentState = EBatteryPlayState::EUnknown; 
	
};
