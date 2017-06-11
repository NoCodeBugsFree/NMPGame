// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class NMPGAME_API APickup : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:

	APickup();

	UFUNCTION(BlueprintPure, Category = "AAA")
	bool IsActive() { return bIsActive; }

	// [Server]
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetIsActive(bool NewActive);
	
	// called when collected
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void WasCollected();
	virtual void WasCollected_Implementation();

	// Server side handling of being picked up 
	UFUNCTION(BlueprintAuthorityOnly, Category = "AAA")
	virtual void PickedUpBy(APawn* Pawn);
	
protected:

	// True when the pickup can be used, false when the pickups deactivated
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;

	// Called whenever bIsActive updated
	UFUNCTION()
	virtual void OnRep_IsActive();

	// the pawn who picked up the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA")
	APawn* PickupInstigator;

private:

	// Client-side handling of being picked up
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnPickedUpBy(APawn* Pawn);
	void MulticastOnPickedUpBy_Implementation(APawn* Pawn);
	
};
