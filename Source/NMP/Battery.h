// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "Battery.generated.h"

/**
 * 
 */
UCLASS()
class NMP_API ABattery : public ABasePickup
{
	GENERATED_BODY()

	/* Emitter Template */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* EmitterTemplate;

	/** spawned emitter reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* Emitter;
	
public:

	ABattery();

	virtual void BeginPlay() override;

	/** disabled at BeginPlay  */
	virtual void Tick(float DeltaSeconds) override;
	
	/** [BlueprintNativeEvent] calls when pickup is collected */
	virtual void WasCollected_Implementation() override;
	
	/* calls by server to client-side handle pickup event */
	virtual void MulticastPickedUpBy_Implementation(class ANMP_Player* PickupPawn) override;
	
private:
	
	/** pickup power to add */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Power = 200.f;	

	/** emitter target point to attach  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FVector TargetPoint;

public:

	/** returns battery power **/
	FORCEINLINE float GetPower() const { return Power; }
};
