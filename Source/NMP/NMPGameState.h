// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NMPGameState.generated.h"

UENUM(BlueprintType)
enum class EPlayState : uint8
{
	PS_Unknown	 UMETA(DisplayName = "Unknown"),
	PS_Win		 UMETA(DisplayName = "Win"),
	PS_Lose		 UMETA(DisplayName = "Lose"),
	PS_Playing   UMETA(DisplayName = "Playing")
};

/**
 * 
 */
UCLASS()
class NMP_API ANMPGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	/** calls to change play state */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetPlayState(EPlayState NewState) { PlayState = NewState; }
	
	/** calls to set the player name who won the game */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetWinningPlayerName(FString Name) { WinningPlayerName = Name; }

private:

	/** current game state - starts at Unknown state */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EPlayState PlayState = EPlayState::PS_Unknown;

	/** the name of the player who won the game  */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FString WinningPlayerName;

public:

	/** returns current play state **/
	FORCEINLINE EPlayState GetPlayState() const { return PlayState; }
	/** returns current play state **/
	FORCEINLINE FString GetWinningPlayerName() const { return WinningPlayerName; }
};
