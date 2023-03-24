// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DemoPlayerState.h"
#include "Demo/Public/Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/AttributeSets/CharacterAttributeSetBase.h"

ADemoPlayerState::ADemoPlayerState()
{
	AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

	//NetUpdateFrequency = 100.f

	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//AbilitySystemComponent->SetIsReplicted(true);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* ADemoPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSetBase* ADemoPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool ADemoPlayerState::IsAlive() const
{
	return GetHealth() > 0.f;
}

void ADemoPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	//
}

float ADemoPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float ADemoPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float ADemoPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float ADemoPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

int32 ADemoPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetLevel();
}

void ADemoPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ADemoPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &ADemoPlayerState::MaxHealthChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &ADemoPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &ADemoPlayerState::MaxManaChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetLevelAttribute()).AddUObject(this, &ADemoPlayerState::CharacterLevelChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADemoPlayerState::StunTagChanged);
	}
}

void ADemoPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("HealthChanged"));
}

void ADemoPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("MaxHealthChanged"));
}

void ADemoPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("ManaChanged"));
}

void ADemoPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("MaxManaChanged"));
}

void ADemoPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterLevelChanged"));
}

void ADemoPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCancelByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);

		UE_LOG(LogTemp, Warning, TEXT("StunTagChanged"));
	}

	
}
