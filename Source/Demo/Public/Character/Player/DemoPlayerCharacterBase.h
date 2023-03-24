// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DemoCharacterBase.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"

#include "DemoPlayerCharacterBase.generated.h"

UCLASS()
class DEMO_API ADemoPlayerCharacterBase : public ADemoCharacterBase
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Demo|Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Demo|Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Ability Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability5Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* Ability6Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* ConfirmAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Demo|Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelAction;
	
public:
	ADemoPlayerCharacterBase(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	


protected:
	void SendLocalInputToASC(bool bIsPressed, const DemoAbilityID AbilityInputID);

	bool ASCInputBound = false;

	FGameplayTag DeadTag;
	FGameplayTag StunTag;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Jump input */
	void HandleJumpActionPressed();
	void HandleJumpActionReleased();

	/** Called for Abilities input */
	void HandleAbility1ActionPressed();
	void HandleAbility1ActionReleased();

	void HandleAbility2ActionPressed();
	void HandleAbility2ActionReleased();

	void HandleAbility3ActionPressed();
	void HandleAbility3ActionReleased();

	void HandleAbility4ActionPressed();
	void HandleAbility4ActionReleased();

	void HandleAbility5ActionPressed();
	void HandleAbility5ActionReleased();

	void HandleAbility6ActionPressed();
	void HandleAbility6ActionReleased();

	void HandleConfirmActionPressed();
	void HandleConfirmActionReleased();

	void HandleCancelActionPressed();
	void HandleCancelActionReleased();

	virtual void OnRep_PlayerState() override;

	void BindASCInput();

	void InitializeStartingValues(class ADemoPlayerState* PS);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	UFUNCTION(BlueprintCallable, Category = "Demo|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "Demo|Camera")
	FVector GetStartingCameraBoomLocation();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Demo|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "Demo|Camera")
	FVector StartingCameraBoomLocation;
};
