// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Monsters/EnemyPanchi.h"

#include "Components/WidgetComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Enemy/Controller/EnemyController.h"

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
}

void AEnemyPanchi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UIFindLookPlayer(HealthBarWidget);
}

void AEnemyPanchi::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeStartingValues();

	AddStartupEffects();
	AddCharacterAbilities();
}

void AEnemyPanchi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnemyPanchi::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEnemyPanchi::InitializeStartingValues()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Panchi InitializeStartingValues"));

	if(EnemyController)
	{
		AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(EnemyController->GetAbilitySystemComponent());

		AbilitySystemComponent->InitAbilityActorInfo(EnemyController, this);

		AttributeSetBase = EnemyController->GetAttributeSetBase();

		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		InitializeAttributes();

		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Panchi InitializeStartingValues is Failed"));
	}
	
	
}

UAnimMontage* AEnemyPanchi::DefaultAttack()
{
	return Super::DefaultAttack();
}


void AEnemyPanchi::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
	
}
