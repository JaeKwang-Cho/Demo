// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterAnimInstanceBase.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UEnemyAnimInstance : public UCharacterAnimInstanceBase
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayDefaultAttackMontage(float PlayRate);

	void PlayDefaultThrowMontage(float PlayRate);

private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= Movement, meta = (AllowPrivateAccess = "true"))
	class AEnemyCharacter* Enemy;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= Movement, meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montage",meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DefaultAttackMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montage",meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DefaultThrowMontage;

	UFUNCTION()
	void AnimNotify_DefaultThrow_GrabRock() const;

	UFUNCTION()
	void AnimNotify_DefaultThrow_ThrowRock() const;

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_ThrowEnd();
};
