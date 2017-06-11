// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPGame.h"
#include "Pickup.h"
//#include "Net/UnrealNetwork.h"

APickup::APickup()
{
	// do not need to tick every frame
	PrimaryActorTick.bCanEverTick = false;

	// Tell the engine to replicate this actor
	bReplicates = true;

	// disable by default
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	
	// [Server]
	if (Role == ROLE_Authority)
	{
		bIsActive = true;
	}
}

void APickup::SetIsActive(bool NewPickupState)
{
	// [Server]
	if (Role == ROLE_Authority)
	{
		bIsActive = NewPickupState;
	}
}

void APickup::OnRep_IsActive()
{
	UE_LOG(LogTemp, Error, TEXT("APickup::OnRep_IsActive()"));
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APickup, bIsActive);
}

void APickup::WasCollected_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("APickup::WasCollected_Implementation() %s"), *GetName());
}

// [Server]
void APickup::PickedUpBy(APawn* Pawn)
{
	// [Server]
	if (Role == ROLE_Authority)
	{
		// store the pawn who picked up the pickup (on server)
		PickupInstigator = Pawn;

		// Notify Clients of the picked up actions
		MulticastOnPickedUpBy(Pawn);
	}
}

// [Client]
void APickup::MulticastOnPickedUpBy_Implementation(APawn* Pawn)
{
	// store the pawn who picked up the pickup(on client)
	PickupInstigator = Pawn;

	// fire the BlueprintNativeEvent which itself cannot be Replicated
	WasCollected();
}
