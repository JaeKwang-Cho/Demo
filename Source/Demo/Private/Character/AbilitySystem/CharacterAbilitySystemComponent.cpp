// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AbilitySystem/CharacterAbilitySystemComponent.h"

void UCharacterAbilitySystemComponent::ReceiveDamage(UCharacterAbilitySystemComponent* sourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(sourceASC, UnmitigatedDamage, MitigatedDamage);


}