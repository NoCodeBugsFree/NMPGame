// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPGame.h"
#include "SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickup.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Disable tick
	PrimaryActorTick.bCanEverTick = false;

	// [Server]
	if (Role == ROLE_Authority)
	{
		WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
		RootComponent = WhereToSpawn;

		SpawnDelayRangeLow = 1.f;
		SpawnDelayRangeHight = 4.5f;
	}
}

FVector ASpawnVolume::GetRandomPointInVolume()
{
	if (WhereToSpawn)
	{
		FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
		FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
		return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
	}
	return FVector();
}

// [Server]
void ASpawnVolume::SetSpawningActive(bool bShouldSpawn)
{
	if (Role == ROLE_Authority)
	{
		if (bShouldSpawn)
		{
			// start spawning
			SpawnDelay = FMath::RandRange(SpawnDelayRangeLow, SpawnDelayRangeHight);

			// set the timer to start spawning next pickup once 
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
		}
		else
		{
			// stop spawning
			GetWorldTimerManager().ClearTimer(SpawnTimer);
		}
	}
}

// [Server]
void ASpawnVolume::SpawnPickup()
{
	// [Server]
	if (Role == ROLE_Authority && WhatToSpawn)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
		
			FVector SpawnLocation = GetRandomPointInVolume();
			FRotator SpawnRotation = UKismetMathLibrary::RandomRotator();
			APickup* Spawned = World->SpawnActor<APickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			// random delay in range and spawn next pickup
			SetSpawningActive(true);
		}
	}
}