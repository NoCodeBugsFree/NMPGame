// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class NMPGAME_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
		
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	/** Returns random point in Box Component  */
	UFUNCTION(BlueprintPure, Category = "AAA")
	FVector GetRandomPointInVolume();

	/** Enable/disable this actor's spawn ability  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetSpawningActive(bool bShouldSpawn);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	TSubclassOf<class APickup> WhatToSpawn;

	FTimerHandle SpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	float SpawnDelayRangeLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	float SpawnDelayRangeHight;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	// [Server]
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnPickup();

	// Actual time (seconds) before spawning the next time
	float SpawnDelay;
	
};
