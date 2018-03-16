// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePickup.h"
#include "ConstructorHelpers.h"
#include "NMP_Player.h"
#include "Net/UnrealNetwork.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = false;

	/**  set the mesh asset */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (Sphere.Succeeded())
	{
		GetStaticMeshComponent()->SetStaticMesh(Sphere.Object);
	}

	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	SetReplicates(true);

	bActive = false;
}

void ABasePickup::WasCollected_Implementation()
{
	
}

void ABasePickup::MulticastPickedUpBy_Implementation(ANMP_Player* PickupPawn)
{
	PickupInstigator = PickupPawn;

	/** [BlueprintNativeEvent] play FX client-size  */
	WasCollected();

}

/** Returns properties that are replicated for the lifetime of the actor channel */
void ABasePickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePickup, bActive);
	
}