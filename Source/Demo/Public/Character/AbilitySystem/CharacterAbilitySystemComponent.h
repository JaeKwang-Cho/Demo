// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CharacterAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceiveDamageDelegate, UCharacterAbilitySystemComponent*, sourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

UCLASS()
class DEMO_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;

	FReceiveDamageDelegate ReceivedDamage;

	virtual void ReceiveDamage(UCharacterAbilitySystemComponent* sourceASC, float UnmitigatedDamage, float MitigatedDamage);
	
};
