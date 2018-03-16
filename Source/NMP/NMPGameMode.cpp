// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/HUD.h"
#include "ConstructorHelpers.h"
#include "NMP_Player.h"
#include "Kismet/GameplayStatics.h"
#include "SpawningVolume.h"
#include "EngineUtils.h"

ANMPGameMode::ANMPGameMode()
{
	/**  set a player pawn asset */
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/BP_NMP_Character"));
	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	/** set a game state asset  */
	static ConstructorHelpers::FClassFinder<AGameState> GameStateBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/BP_NMP_GameState"));
	if (GameStateBPClass.Class)
	{
		GameStateClass = GameStateBPClass.Class;
	}

	/** set a HUD asset  */
	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/BP_NMPHUD"));
	if (HUDBPClass.Class)
	{
		HUDClass = HUDBPClass.Class;
	}
}

void ANMPGameMode::BeginPlay()
{
	Super::BeginPlay();

	/** set the game state reference  */
	NMPGameState = GetGameState<ANMPGameState>();

	/** starts playing  */
	SetPlayState(EPlayState::PS_Playing);

	/** Set the decay players timer */
	GetWorldTimerManager().SetTimer(DecayTimer, this, &ANMPGameMode::DecayPlayers, DecayDelay, true);
	
	/** collect all spawning volumes  */
	for (TActorIterator<ASpawningVolume> Iter(GetWorld()); Iter; ++Iter)
	{
		if ((*Iter))
		{
			SpawningVolumes.AddUnique(*Iter);
		}
	}

	/** starts spawning  */
	ActivateSpawningVolumes(true);
}

void ANMPGameMode::SetPlayState(EPlayState NewState)
{
	if (NMPGameState)
	{
		NMPGameState->SetPlayState(NewState);
		
		/** handle new state  */
		switch (NewState)
		{

		/** WIN  */
		case EPlayState::PS_Win:

			/** stop spawn  */
			ActivateSpawningVolumes(false);

			/** stop draining power  */
			GetWorldTimerManager().ClearTimer(DecayTimer);
			break;

		/** LOSE  */
		case EPlayState::PS_Lose:

			/** stop spawn  */
			ActivateSpawningVolumes(false);

			/** stop draining power  */
			GetWorldTimerManager().ClearTimer(DecayTimer);

			break;

		/** PLAYING  */
		case EPlayState::PS_Playing:

			/** starts spawning  */
			ActivateSpawningVolumes(true);
			break;

		default:
			break;	

		}
	}
}

void ANMPGameMode::ActivateSpawningVolumes(bool bActive)
{
	for (ASpawningVolume* SpawningVolume : SpawningVolumes)
	{
		if (SpawningVolume)
		{
			SpawningVolume->Activate(bActive);
		}
	}
}

void ANMPGameMode::WinTheGame(ANMP_Player* Winner)
{
	if (NMPGameState && Winner)
	{
		NMPGameState->SetWinningPlayerName(Winner->GetName());
	}
}

void ANMPGameMode::DecayPlayers()
{
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{	
		if(APawn* Pawn = Cast<APawn>(*It))
		{
			if(ANMP_Player* NMP_Player = Cast<ANMP_Player>(Pawn))
			{
				NMP_Player->AddPower(-DecayPower);

				/** if we have zero power and we still alive  */
				if (NMP_Player->GetCurrentPower() <= 0.f && !NMP_Player->IsDead())
				{
					/** mark this player as dead  */
					NMP_Player->SetDead(true);
					/** decrease alive players counter  */
					PlayersALive--;

					/** lose the game  */
					if (PlayersALive <= 0)
					{
						SetPlayState(EPlayState::PS_Lose);
					}
				}
			}
		}
	}
}
