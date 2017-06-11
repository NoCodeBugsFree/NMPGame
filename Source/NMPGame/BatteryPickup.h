// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class NMPGAME_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:

	ABatteryPickup();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Uses in Tick when Tick if enabled */
 	UFUNCTION()
 	void AttachEmmiterToCharacter();
 	
	// Server side handling of being picked up by the child class
	UFUNCTION(BlueprintAuthorityOnly, Category = "AAA")
	virtual void PickedUpBy(APawn* Pawn) override;

	/** Returns battery power */
	float GetPower() const { return BatteryPower; }
	
	// override the base class version
	virtual void WasCollected_Implementation() override;
	
	/** Sets the pawn to attach emitter  */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPawnToAttach(ACharacter* Character); 
	void MulticastSetPawnToAttach_Implementation(ACharacter* Character) { CharacterToAttach = Character; }

protected:

	/** Current Battery Power */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "AAA", meta = (BlueprintProtected = "true"))
	float BatteryPower;

	/** Lightning Emitter Template  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* EmitterTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	float ChangeBoneNameDelayMin = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	float ChangeBoneNameDelayMax= 0.5f;

private:

	/** Timer that changes bones to attach emitter  */
	FTimerHandle ChangeBoneNameTimer;

	/** Sets next random bone name to attach emitter   */
	void ChangeBoneName();

	/** Returns  Random Bone Name */
	FName GetRandomBoneName();

	/** Defaults Bones to attach emitter  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	TArray<FName> BoneNamesToAttach = { "spine_03", "pelvis", "clavicle_r", "hand_r", "hand_l", "foot_r", "head" };
	
	/** Current bone name  */
	FName BoneName;
	
	/** Character reference to attach emitter  */
	UPROPERTY()
	ACharacter* CharacterToAttach;

	/** Emitter Reference  */
	UPROPERTY()
	UParticleSystemComponent* Emitter;
	
};
