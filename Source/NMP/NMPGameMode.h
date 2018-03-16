// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NMPGameState.h"
#include "NMPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NMP_API ANMPGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	ANMPGameMode();

	virtual void BeginPlay() override;

	// -----------------------------------------------------------------------------------
	
	/** calls to add / reduce alive players amount */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void AddPlayersAlive(int32 Amount) { PlayersALive += Amount; }

	/** calls to change play state in game state and handle it */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetPlayState(EPlayState NewState);

	/** calls to start / stop spawning  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void ActivateSpawningVolumes(bool bActive);
	
	/** calls to set winner name in the game state */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void WinTheGame(ANMP_Player* Winner);

private:

	/** calls by decay timer to deplete players power  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void DecayPlayers();

	// -----------------------------------------------------------------------------------

	/** game state reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ANMPGameState* NMPGameState;

	/** players decay timer triggering time  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DecayDelay = 1.f;
	
	/** the amount of power to decay to each player per Decay Timer Tick  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DecayPower = 100.f;

	/** timer that cause decay logic to all players  */
	UPROPERTY()
	FTimerHandle DecayTimer;

	/** the amount of alive players in the world  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 PlayersALive = 0;

	/** all spawning volumes in the world  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<class ASpawningVolume*> SpawningVolumes;
	
public:

	/** returns alive players amount **/
	FORCEINLINE int32 GetPlayersALive() const { return PlayersALive; }
};
