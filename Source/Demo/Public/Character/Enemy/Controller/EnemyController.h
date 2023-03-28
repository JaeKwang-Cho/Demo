// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"

#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API AEnemyController : public AAIController, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AEnemyController(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite,Category = "AI Behavior",meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent*  BlackboardComponent;
	
	UPROPERTY(BlueprintReadWrite,Category = "AI Behavior",meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

public:
	UPROPERTY(BlueprintReadWrite)
	class ADemoCharacterBase* PlayerCharacter;
	
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;
	
	class UAISenseConfig_Sight* Sight;
	
	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	void SetSensedTarget(APawn* NewTarget);

	class AEnemyCharacter* EnemyCharacter;

	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent() const {return BlackboardComponent;}

	/*
	 * GAS
	 */

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UCharacterAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|GAS")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|GAS|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintCallable, Category = "Demo|GAS|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|GAS|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|GAS|Attributes")
	float GetMana() const;
	
	UFUNCTION(BlueprintCallable, Category = "Demo|GAS|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|GAS|Attributes")
	int32 GetCharacterLevel() const;

protected:

	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UCharacterAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
