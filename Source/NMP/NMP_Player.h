// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NMP_Player.generated.h"

UCLASS()
class NMP_API ANMP_Player : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* sphere collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	/* character's body dynamic material */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DynamicMaterial;

public:

	// Sets default values for this character's properties
	ANMP_Player();

	/** calls to add some power to player */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void AddPower(float PowerToAdd);

	/** calls to set new dead/alive state */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void SetDead(bool bNewDead) { bDead = bNewDead; }
	
protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	
	/** Called for forwards/backward input */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveForward(float Value);

	/** Called for side to side input */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveRight(float Value);

	/** calls when client tries to collect pickups  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void CollectPickups();

	/* calls to server collect pickups */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerCollectPickups();
	bool ServerCollectPickups_Validate(){ return true; }
	void ServerCollectPickups_Implementation();

	/* calls to change body color */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastChangeBodyColor();
	void MulticastChangeBodyColor_Implementation();

	/* calls to die when we have not more power */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDie();
	void MulticastDie_Implementation();
	
	// -----------------------------------------------------------------------------------

	/** Remember! Listen Server doesn't call it automatically like in BP version ! Call it manually */
	/* Called whenever CurrentPower updated */
	UFUNCTION()
	virtual void OnRep_CurrentPower();
	
	/** current power level for this character */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float CurrentPower = 2000.f;

	// -----------------------------------------------------------------------------------

	/** character starts with this amount of power */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float InitialPower = 2000.f;

	/** simple twice initial power to win the game */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float PowerToWin = 2.f * InitialPower;

	/** color when we are close to win the game (charged) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor WinColor = FLinearColor::Red;

	/** color when we are close to lose the game (depleted) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor LoseColor = FLinearColor::Blue;

	/** character starts with this amount of speed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float InitialSpeed = 600.f;

	/** game mode reference  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ANMPGameMode* NMPGameMode;

	/** shows whether character is alive  or dead  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bDead : 1;
	
public:
	/** returns whether character is dead or alive  */
	FORCEINLINE bool IsDead() const { return bDead; }
	/** returns current power  */
	FORCEINLINE float GetCurrentPower() const { return CurrentPower; }
	/** Returns power percentage **/
	FORCEINLINE float GetPowerPercent() const { return CurrentPower / PowerToWin; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
