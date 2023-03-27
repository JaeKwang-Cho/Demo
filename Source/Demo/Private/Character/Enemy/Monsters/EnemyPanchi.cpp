// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Monsters/EnemyPanchi.h"

#include "Components/WidgetComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/DemoPlayerState.h"
#include "Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/AttributeSets/CharacterAttributeSetBase.h"

AEnemyPanchi::AEnemyPanchi(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	//HealthBarWidget->SetupAttachment(GetRootComponent());
	
	Visitors.insert(std::make_pair("Desc",std::make_shared<DescVisitor>()));
	Visitors.insert(std::make_pair("DefaultAttack",std::make_shared<AttackVisitor>()));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	StunTag = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"));
}

void AEnemyPanchi::BeginPlay()
{
	Super::BeginPlay();

	ADemoPlayerState* PS = GetPlayerState< ADemoPlayerState>();
	if (PS)
	{
		InitializeStartingValues(PS);

		AddStartupEffects();
		AddCharacterAbilities();
	}
}

void AEnemyPanchi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UIFindLookPlayer(HealthBarWidget);
}

void AEnemyPanchi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnemyPanchi::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEnemyPanchi::InitializeStartingValues(ADemoPlayerState* PS)
{
	UE_LOG(LogTemp, Warning, TEXT("Panchi InitializeStartingValues"));
	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());

	AbilitySystemComponent.Get()->InitAbilityActorInfo(PS, this);

	AttributeSetBase = PS->GetAttributeSetBase();

	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

	InitializeAttributes();

	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());
}

void AEnemyPanchi::DefaultAttack()
{
	Super::DefaultAttack();
}


void AEnemyPanchi::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
	
}
