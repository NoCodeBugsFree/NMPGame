// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "NMPGame.h"
#include "NMPGameGameMode.h"
#include "NMPGameCharacter.h"
#include "NMPGameGameState.h"
#include "SpawnVolume.h"

ANMPGameGameMode::ANMPGameGameMode()
{
	// HUD
	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDClass(TEXT("/Game/ThirdPersonCPP/Blueprints/BP_NMPGameHUD"));
	if (PlayerHUDClass.Class != NULL)
	{
		HUDClass = PlayerHUDClass.Class;
	}

	// GameState
	GameStateClass = ANMPGameGameState::StaticClass();
	
	DecayRate = 0.02f;
	PowerDrainDelay = 0.25f;
	PowerToWinMultiplier = 1.25f;
}

void ANMPGameGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* const World = GetWorld();
	check(World);
	ANMPGameGameState* MyGameState = Cast<ANMPGameGameState>(GameState);
	check(MyGameState);

	// reset stats
	DeadPlayerCount = 0;
	
	// find all spawn volume actors and store them into array
	InitializeSpawnVolumes(World);

	// start playing
	HandleNewState(EBatteryPlayState::EPlaying);
	
	// find character and initialize PowerToWin variable in GameState
	SetPowerToWin(World, MyGameState);
}

void ANMPGameGameMode::SetPowerToWin(UWorld* const World, ANMPGameGameState* MyGameState)
{
	for (FConstControllerIterator It = World->GetControllerIterator(); It; It++)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (ANMPGameCharacter* BatteryCharacter = Cast<ANMPGameCharacter>(PlayerController->GetPawn()))
			{
				MyGameState->PowerToWin = BatteryCharacter->GetInitialPower() * PowerToWinMultiplier;

				// stop when we found first BatteryCharacter
				break;
			}
		}
	}
}

void ANMPGameGameMode::InitializeSpawnVolumes(UWorld* const World)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}
}

void ANMPGameGameMode::DrainPowerOverTime()
{
	UWorld* const World = GetWorld();
	check(World);

	ANMPGameGameState* MyGameState = Cast<ANMPGameGameState>(GameState);
	check(MyGameState);
	
	// find all BatteryCharacters in the World
	for (FConstControllerIterator It = World->GetControllerIterator(); It; It++)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (ANMPGameCharacter* BatteryCharacter = Cast<ANMPGameCharacter>(PlayerController->GetPawn()))
			{
				HandleDrainForThisCharacter(BatteryCharacter, MyGameState);
			}
		}
	}
}

void ANMPGameGameMode::HandleDrainForThisCharacter(ANMPGameCharacter* BatteryCharacter, ANMPGameGameState* MyGameState)
{
	// Win
	if (BatteryCharacter->GetCurrentPower() > MyGameState->PowerToWin)
	{
		// set the winning player name 
		MyGameState->WinningPlayerName = BatteryCharacter->GetName();
		HandleNewState(EBatteryPlayState::EWon);
	}
	// UpdatePower
	else if (BatteryCharacter->GetCurrentPower() > 0)
	{
		BatteryCharacter->UpdatePower(-PowerDrainDelay * DecayRate * (BatteryCharacter->GetInitialPower()));
	}
	// Die
	else
	{
		// die
		BatteryCharacter->MulticastOnPlayerDeath();

		DeadPlayerCount++;

		// see if this is the last player 
		if (DeadPlayerCount >= GetNumPlayers())
		{
			// last player died - game over 
			HandleNewState(EBatteryPlayState::EGameOver);
		}
	}
}

void ANMPGameGameMode::HandleNewState(EBatteryPlayState NewState)
{
	UWorld* const World = GetWorld();
	check(World);
	ANMPGameGameState* MyGameState = Cast<ANMPGameGameState>(GameState);
	check(MyGameState);

	// Only transition if it is a new state
	if (NewState != MyGameState->GetCurrentState())
	{
		// update the state, so clients know about transition
		MyGameState->SetCurrentState(NewState);

		switch (NewState)
		{
			case EBatteryPlayState::EPlaying:

				// Activate SpawnVolumes
				for (ASpawnVolume* SpawnVolume : SpawnVolumeActors)
				{
					SpawnVolume->SetSpawningActive(true);
				}

				// start draining power
				GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &ANMPGameGameMode::DrainPowerOverTime, PowerDrainDelay, true);

				break;

			case EBatteryPlayState::EGameOver:

				// Deactivate SpawnVolumes
				for (ASpawnVolume* SpawnVolume : SpawnVolumeActors)
				{
					SpawnVolume->SetSpawningActive(false);
				}

				// Stop Draining power
				GetWorldTimerManager().ClearTimer(PowerDrainTimer);

				break;

			case EBatteryPlayState::EWon:

				// Deactivate SpawnVolumes
				for (ASpawnVolume* SpawnVolume : SpawnVolumeActors)
				{
					SpawnVolume->SetSpawningActive(false);
				}

				// Stop Draining power
				GetWorldTimerManager().ClearTimer(PowerDrainTimer);

				break;

			default:
			case EBatteryPlayState::EUnknown:
				break;
			
		}
	}
}

