// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawningVolume.generated.h"

UCLASS()
class NMP_API ASpawningVolume : public AActor
{
	GENERATED_BODY()
	
	/* box collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
	
public:	

	// Sets default values for this actor's properties
	ASpawningVolume();

	/** calls to activate / deactivate spawning volume  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void Activate(bool bActive) { bShouldSpawn = bActive; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/** timer that handles spawning pickups  */
	UPROPERTY()
	FTimerHandle SpawnTimer;

	/** minimum time to next spawn  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MinSpawnDelay = 1.f;

	/** maximum time to next spawn  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxSpawnDelay = 5.f;

	/** shows whether this spawning volume is active or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bShouldSpawn : 1;

	/** pickup template  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABasePickup> WhatToSpawn;

	// -----------------------------------------------------------------------------------

	/** calls to get a random point in box  */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	FVector GetRandomPointInVolume() const;

	/** calls to spawn a pickup */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void Spawn();

	
};
