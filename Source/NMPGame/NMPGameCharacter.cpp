// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "NMPGame.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "NMPGameCharacter.h"
#include "Pickup.h"
#include "BatteryPickup.h"
#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////
// ANMPGameCharacter

ANMPGameCharacter::ANMPGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	

	// Create a Collection SphereRadius
	CollectionSphereRadius = 200.f;

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(CollectionSphereRadius);

	InitialPower = 2000.f;
	CurrentPower = InitialPower;
	BaseSpeed = 10.f;
	SpeedFactor = 0.75f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ANMPGameCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	/** Create Dynamic Material  */ 
	DynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
}

void ANMPGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANMPGameCharacter, CollectionSphereRadius);
	DOREPLIFETIME(ANMPGameCharacter, InitialPower);
	DOREPLIFETIME(ANMPGameCharacter, CurrentPower);
}

// [Server]
void ANMPGameCharacter::UpdatePower(float DeltaPower)
{
	// [Server]
	if (Role == ROLE_Authority)
	{
		// update current power
		CurrentPower += DeltaPower;

		// set movement speed based on power level
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * CurrentPower;
		
		// fake the rep notify (listen server does not get the RepNotify automatically)
		OnRep_CurrentPower();
	}
}

void ANMPGameCharacter::OnRep_CurrentPower()
{
	// Disable original
	// PowerChangeEffect();

	// cpp final version
	PowerChangeEffect_Cpp();
}

// default BP Prototype
void ANMPGameCharacter::PowerChangeEffect_Implementation()
{

}

// cpp override
void ANMPGameCharacter::PowerChangeEffect_Cpp()
{
	if (DynamicMaterial)
	{
		float Alpha = FMath::Clamp(GetCurrentPower() / GetInitialPower(), 0.f, 1.f);
		FLinearColor ColorToSet = UKismetMathLibrary::LinearColorLerp(InitialColor, ExhaustedColor, Alpha);
		DynamicMaterial->SetVectorParameterValue("BodyColor", ColorToSet);
	}
}

void ANMPGameCharacter::MulticastOnPlayerDeath_Implementation()
{
	// disconnect the controller from the pawn
	DetachFromControllerPendingDestroy();

	static FName CollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionProfileName(CollisionProfileName);
	
	SetActorEnableCollision(true);

	// rag doll  (init Physics)
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	// disable movement
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	// disable collisions on the capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANMPGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANMPGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANMPGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANMPGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANMPGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ANMPGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ANMPGameCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANMPGameCharacter::OnResetVR);

	// collecting pickups
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ANMPGameCharacter::CollectPickups);
}

void ANMPGameCharacter::CollectPickups()
{
	// ask the server to collect pickups
	ServerCollectPickups();
}

// [Server]
void ANMPGameCharacter::ServerCollectPickups_Implementation()
{
	// [Server]
	if (Role == ROLE_Authority)
	{
		float TotalPower = 0.f;

		TArray<AActor*> CollectedActors;
		CollectionSphere->GetOverlappingActors(CollectedActors);

		for (AActor* TestActor : CollectedActors)
		{
			// if this is a APickup
			APickup* TestPickup = Cast<APickup>(TestActor);
			if (TestPickup && TestPickup->IsActive() && !TestPickup->IsPendingKill())
			{
				// and if this is ABatteryPickup too
				if (ABatteryPickup* const Battery = Cast<ABatteryPickup>(TestPickup))
				{
					// add battery to TotalPower
					TotalPower += Battery->GetPower();

					// setup the pawn to attach emitter
					Battery->MulticastSetPawnToAttach(this);
				}
				
				// play FX
				TestPickup->PickedUpBy(this);

				// deactivate pickup
				TestPickup->SetIsActive(false);
			}
		}

		// Update Power if it is not nearly Zero
		if (!FMath::IsNearlyZero(TotalPower, 0.001f))
		{
			UpdatePower(TotalPower);
		}
	}
}

void ANMPGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ANMPGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ANMPGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ANMPGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANMPGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANMPGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ANMPGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
