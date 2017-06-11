// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "NMPGameCharacter.generated.h"

UCLASS(config=Game)
class ANMPGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AAA, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

	
public:

	ANMPGameCharacter();
	
	// create dynamic material here
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintPure, Category = "AAA")
	float GetInitialPower() const { return InitialPower; }

	UFUNCTION(BlueprintPure, Category = "AAA")
	float GetCurrentPower() const { return CurrentPower; }

	/**  
	* This updates the character's power level
	* @param DeltaPower this is the amount to change power by, can be + or -
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "AAA")
	void UpdatePower(float DeltaPower);

	// Shut down the pawn and rag doll it on all clients
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnPlayerDeath();
	void MulticastOnPlayerDeath_Implementation();
	 
protected:
	
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// EntryPoint for collection pickups logic - ask the server to collect pickups
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CollectPickups();
	
	// Called on server to process the collection
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCollectPickups();
	bool ServerCollectPickups_Validate() { return true; }
	void ServerCollectPickups_Implementation();
	
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "AAA", meta = (BlueprintProtected = "true"))
	float InitialPower;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category = "AAA")
	float CurrentPower;

	// power level is updated on clients
	UFUNCTION()
	void OnRep_CurrentPower();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA", meta = (BlueprintProtected = "true"))
	float BaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA", meta = (BlueprintProtected = "true"))
	float SpeedFactor;

	// DEPRICATED
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void PowerChangeEffect();
	void PowerChangeEffect_Implementation();

	// OVERRIDE BP Prototype
	void PowerChangeEffect_Cpp();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns Collection Sphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }

private:
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	float CollectionSphereRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	FLinearColor InitialColor = FLinearColor(0.f, 0.11f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	FLinearColor ExhaustedColor = FLinearColor(1.0f, 0.3f, 0.0f);

};