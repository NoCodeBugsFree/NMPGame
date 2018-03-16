// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawningVolume.h"
#include "Components/BoxComponent.h"
#include "Battery.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawningVolume::ASpawningVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* box collision  */
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	
	/**  set what to spawn template */
	static ConstructorHelpers::FClassFinder<ABattery> WhatToSpawnBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/BP_BatteryPickup"));
	if (WhatToSpawnBPClass.Class)
	{
		WhatToSpawn = WhatToSpawnBPClass.Class;
	}
	
	if (Role == ROLE_Authority)
	{
		/** starts deactivated, activates by game mode  */
		bShouldSpawn = false;
	}
}

// Called when the game starts or when spawned
void ASpawningVolume::BeginPlay()
{
	Super::BeginPlay();
	
	// [Server]
	if (Role == ROLE_Authority)
	{
		/** Set SpawnTimer Once */
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawningVolume::Spawn, FMath::RandRange(MinSpawnDelay, MaxSpawnDelay), false);
	}
}

FVector ASpawningVolume::GetRandomPointInVolume() const
{
	if (BoxComponent)
	{
		FVector SpawnOrigin = BoxComponent->Bounds.Origin;
		FVector SpawnExtent = BoxComponent->Bounds.BoxExtent;
		return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
	}
	return FVector();
}

void ASpawningVolume::Spawn()
{
	if (WhatToSpawn && bShouldSpawn && Role == ROLE_Authority)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator; //  GetController();
			
			FVector SpawnLocation = GetRandomPointInVolume();
			FRotator SpawnRotation = UKismetMathLibrary::RandomRotator(true);
		
			ABasePickup* Spawned = World->SpawnActor<ABasePickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			if (Spawned)
			{
				//
			}

			/** reset timer  */
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawningVolume::Spawn, FMath::RandRange(MinSpawnDelay, MaxSpawnDelay), false);
		}
	}
}


