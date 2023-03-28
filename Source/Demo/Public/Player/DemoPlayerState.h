// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"

#include "DemoPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API ADemoPlayerState: public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADemoPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UCharacterAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState|Attributes")
	float GetMana() const;
	
	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|DemoplayerState|Attributes")
	int32 GetCharacterLevel() const;

protected:

	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UCharacterAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	virtual void BeginPlay() override;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
