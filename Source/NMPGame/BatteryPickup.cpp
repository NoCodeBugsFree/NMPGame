// Fill out your copyright notice in the Description page of Project Settings.

#include "NMPGame.h"
#include "BatteryPickup.h"

ABatteryPickup::ABatteryPickup()
{
	// Tick by default, but 
	// Disable Tick() in BeginPlay() and
	// Enable Tick() in WasCollected_Implementation()
	PrimaryActorTick.bCanEverTick = true;
	
	// Keep movement synced from server to clients
	bReplicateMovement = true;

	// Mobility and Physics enable
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

	// Battery Asset
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BatteryMesh(TEXT("StaticMesh'/Game/ExampleContent/Blueprint_Communication/Meshes/SM_Battery_Medium.SM_Battery_Medium'"));
	if(BatteryMesh.Succeeded())
	{
		GetStaticMeshComponent()->SetStaticMesh(BatteryMesh.Object);
	}

	// Emitter Asset
	EmitterTemplate = CreateDefaultSubobject<UParticleSystem>(TEXT("Emmiter"));

 	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("ParticleSystem'/Game/ExampleContent/Effects/ParticleSystems/P_electricity_arc.P_electricity_arc'"));
	if (ParticleSystem.Object)
 	{
		EmitterTemplate = ParticleSystem.Object;
 	}
	
	BatteryPower = 200.f;
}

void ABatteryPickup::BeginPlay()
{
	Super::BeginPlay();

	// disable tick
	SetActorTickEnabled(false);

	// initialize bone name
	BoneName = GetRandomBoneName();
}

void ABatteryPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// when ticking enable only 
	AttachEmmiterToCharacter();
}

void ABatteryPickup::AttachEmmiterToCharacter()
{
	if (CharacterToAttach && Emitter)
	{
		FVector TargetPoint = CharacterToAttach->GetMesh()->GetSocketLocation(BoneName);
		Emitter->SetBeamTargetPoint(0, TargetPoint, 0);
	}
}

// [Server]
void ABatteryPickup::PickedUpBy(APawn* Pawn)
{
	Super::PickedUpBy(Pawn);

	// [Server]
	if (Role == ROLE_Authority)
	{
		// to give clients time to play vfx, etc
		SetLifeSpan(2.f);
	}
}

void ABatteryPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABatteryPickup, BatteryPower);
}

void ABatteryPickup::WasCollected_Implementation()
{
	Emitter = UGameplayStatics::SpawnEmitterAttached(EmitterTemplate, GetStaticMeshComponent(), NAME_None, FVector(ForceInit), FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
	SetActorTickEnabled(true);
	ChangeBoneName();
}

void ABatteryPickup::ChangeBoneName()
{
	BoneName = GetRandomBoneName();
	float Delay = FMath::RandRange(ChangeBoneNameDelayMin, ChangeBoneNameDelayMax);
	GetWorldTimerManager().SetTimer(ChangeBoneNameTimer, this, &ABatteryPickup::ChangeBoneName, Delay, true, Delay);
}

FName ABatteryPickup::GetRandomBoneName()
{
	if (BoneNamesToAttach.Num() > 0)
	{
		return BoneNamesToAttach[FMath::RandRange(0, BoneNamesToAttach.Num() - 1)];
	}
	else return "spine_03";
}
