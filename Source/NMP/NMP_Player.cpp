// Fill out your copyright notice in the Description page of Project Settings.

#include "NMP_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "BasePickup.h"
#include "Battery.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "NMPGameMode.h"

// Sets default values
ANMP_Player::ANMP_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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
	CameraBoom->bDoCollisionTest = true;
												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/* sphere collision  */
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(200.f);

	/** overlap only pickups  */
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	/** class defaults  */
	bDead = false; /** all players starts alive  */
}

void ANMP_Player::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
}

// Called when the game starts or when spawned
void ANMP_Player::BeginPlay()
{
	Super::BeginPlay();

	// [Server]
	if (Role == ROLE_Authority)
	{
		/** set the game mode reference  */
		NMPGameMode = Cast<ANMPGameMode>(GetWorld()->GetAuthGameMode());
		if (NMPGameMode)
		{
			/** add one player to game mode */
			NMPGameMode->AddPlayersAlive(1);
		}
	}
}

// Called to bind functionality to input
void ANMP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/** jump  */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	/** collect  */
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ANMP_Player::CollectPickups);

	/** move  */
	PlayerInputComponent->BindAxis("MoveForward", this, &ANMP_Player::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANMP_Player::MoveRight);

	/** turn  */
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void ANMP_Player::MoveForward(float Value)
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

void ANMP_Player::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
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

void ANMP_Player::CollectPickups()
{
	ServerCollectPickups();
}

void ANMP_Player::ServerCollectPickups_Implementation()
{
	if (Role == ROLE_Authority)
	{
		/** get all pickups in sphere radius  */
		if (SphereComponent)
		{
			TArray<AActor*> Pickups;
			SphereComponent->GetOverlappingActors(Pickups);

			for (AActor* Pickup : Pickups)
			{
				/** cast to base pickup class and use single interface for all pickups  */
				ABasePickup* BasePickup = Cast<ABasePickup>(Pickup);
				if (BasePickup && BasePickup->IsActive() && !BasePickup->IsPendingKill())
				{
					/* calls by server to client-side handle pickup event */
					BasePickup->MulticastPickedUpBy(this);

					/** deactivate  */
					BasePickup->SetActive(false);
				}
			}
		}
	}
}

void ANMP_Player::MulticastChangeBodyColor_Implementation()
{
	float Alpha = FMath::Clamp(CurrentPower / PowerToWin, 0.f, 1.f) + 0.2f;
	FLinearColor ColorToSet = UKismetMathLibrary::LinearColorLerp(LoseColor, WinColor, Alpha);
	DynamicMaterial->SetVectorParameterValue("BodyColor", ColorToSet);
}

void ANMP_Player::MulticastDie_Implementation()
{
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

void ANMP_Player::OnRep_CurrentPower()
{
	// change body color
	MulticastChangeBodyColor();
	
	/** change speed  */
	float Alpha = FMath::Clamp(CurrentPower / PowerToWin, 0.f, 1.f);
	float CurrentSpeed = UKismetMathLibrary::Lerp(InitialSpeed * 0.1, InitialSpeed * 3, Alpha);
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void ANMP_Player::AddPower(float PowerToAdd)
{
	if (Role == ROLE_Authority && NMPGameMode)
	{
		CurrentPower += PowerToAdd;
		CurrentPower = FMath::Clamp(CurrentPower, 0.f, PowerToWin);

		/** Remember! Server doesn't call it automatically like in BP version ! Call it manually */
		OnRep_CurrentPower();
		
		/** win the game  */
		if (CurrentPower >= PowerToWin)
		{
			NMPGameMode->SetPlayState(EPlayState::PS_Win);
			NMPGameMode->WinTheGame(this);
		} 
		else if(CurrentPower <= 0)
		{
			/** die!  */
			MulticastDie();
		}
	}
}

/** Returns properties that are replicated for the lifetime of the actor channel */
void ANMP_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANMP_Player, CurrentPower);
	DOREPLIFETIME(ANMP_Player, PowerToWin);
}