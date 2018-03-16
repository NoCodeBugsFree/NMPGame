// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "BasePickup.generated.h"

/** 
 *  base pick up class work flow
 *	when player try to pick up a Pickup in  specified Sphere radius
 *  ServerCollectPickups() will calls MulticastPickedUpBy() which calls WasCollected [BlueprintNativeEvent]
 *  
 *	create child class and override WasCollected event whatever you need
 *	handle FX logic
 *  handle Server logic (e.g. look BP_BadBattery)
 */
UCLASS()
class NMP_API ABasePickup : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:

	ABasePickup();

	/** calls to set new pickup state */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetActive(bool NewActive) { bActive = NewActive; }
	
	/** calls when pickup is collected to play FX */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void WasCollected();
	virtual void WasCollected_Implementation();

	/* calls by server to client-side handle pickup event */
	UFUNCTION(NetMulticast, Reliable) // Unreliable
	void MulticastPickedUpBy(class ANMP_Player* PickupPawn);
	virtual void MulticastPickedUpBy_Implementation(class ANMP_Player* PickupPawn);

protected:

	/** shows whether this pickup is active or not  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bActive : 1;

	/** pawn that pickup the pickup  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ANMP_Player* PickupInstigator;
	
public:

	/** returns pickup's active state **/
	FORCEINLINE bool IsActive() const { return bActive; }
};
