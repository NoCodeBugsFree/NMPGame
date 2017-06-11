// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "NMPGameGameState.h"
#include "NMPGameGameMode.generated.h"

UCLASS(minimalapi)
class ANMPGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	ANMPGameGameMode();

	virtual void BeginPlay() override;

	void SetPowerToWin(UWorld* const World, ANMPGameGameState* MyGameState);
	

	UFUNCTION(BlueprintPure, Category = "AAA")
	float GetDecayRate() const { return DecayRate; }

	UFUNCTION(BlueprintPure, Category = "AAA")
	float GetPowerToWinMultiplier() const { return PowerToWinMultiplier; }

protected:
	
	// Lose power rate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AAA", meta = (BlueprintProtected = "true"))
	float DecayRate;

	FTimerHandle PowerDrainTimer;
	
	// How many times per second to update power and check game rules
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AAA", meta = (BlueprintProtected = "true"))
	float PowerDrainDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAA", meta = (BlueprintProtected = "true"))
	float PowerToWinMultiplier;

	// Dead Players
	int32 DeadPlayerCount = 0;

private:

	void InitializeSpawnVolumes(UWorld* const World);

	/** All Spawn Volume Actors in the world  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	TArray<class ASpawnVolume*> SpawnVolumeActors;

	void DrainPowerOverTime();

	void HandleDrainForThisCharacter(class ANMPGameCharacter* BatteryCharacter, class ANMPGameGameState* MyGameState);
	
	void HandleNewState(EBatteryPlayState NewState);
};



