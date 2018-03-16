// Fill out your copyright notice in the Description page of Project Settings.

#include "Battery.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/EngineTypes.h"
#include "NMP_Player.h"

ABattery::ABattery()
{
	PrimaryActorTick.bCanEverTick = true;

	/**  set the battery mesh asset */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BatteryMesh(TEXT("/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Battery_Medium"));
	if (BatteryMesh.Succeeded())
	{
		GetStaticMeshComponent()->SetStaticMesh(BatteryMesh.Object);
	}

	/** set the emitter asset  */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Game/ExampleContent/Effects/ParticleSystems/P_electricity_arc"));
	if (ParticleSystem.Object)
	{
		EmitterTemplate = ParticleSystem.Object;
	}
	
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	/** all batteries start active  */
	if (Role == ROLE_Authority)
	{
		bActive = true;
	}

	SetReplicateMovement(true);
}

void ABattery::BeginPlay()
{
	Super::BeginPlay();

	// disable tick
	SetActorTickEnabled(false);
}

void ABattery::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (Emitter && PickupInstigator)
	{
		/** update target point this frame  */
		TargetPoint = PickupInstigator->GetMesh()->GetSocketLocation("Spine_03");
		
		/** set beam target  */
		Emitter->SetBeamTargetPoint(0, TargetPoint, 0);
	}
}

void ABattery::WasCollected_Implementation()
{
	/** don't need default   */
	// Super::WasCollected_Implementation();

	// Spawn Emitter Attached
	if (EmitterTemplate)
	{
		Emitter = UGameplayStatics::SpawnEmitterAttached(EmitterTemplate, GetStaticMeshComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
	}

	/** start attaching emitter  */
	SetActorTickEnabled(true);
	
	// [Server]
	if (Role == ROLE_Authority && PickupInstigator)
	{
		PickupInstigator->AddPower(Power);
		
		/** get some time to play FX  */
		SetLifeSpan(2.0);
	}
}

void ABattery::MulticastPickedUpBy_Implementation(class ANMP_Player* PickupPawn)
{
	Super::MulticastPickedUpBy_Implementation(PickupPawn);
	
}