// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Controller/EnemyController.h"

#include "AbilitySystemComponent.h"
#include "Character/Enemy/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Character/Player/DemoPlayerCharacterBase.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Demo/Public/Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/AttributeSets/CharacterAttributeSetBase.h"

#include "Character/Enemy/EnemyCharacter.h"


AEnemyController::AEnemyController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
	
	SetPerceptionComponent(*ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this,TEXT("AIPerception Component")));

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	Sight->SightRadius = 500.f;
	Sight->LoseSightRadius = Sight->SightRadius + 100.f;
	Sight->PeripheralVisionAngleDegrees =75.f;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	GetAIPerceptionComponent()->ConfigureSense(*Sight);
	GetAIPerceptionComponent()->SetDominantSense(Sight->GetSenseImplementation());

	/* GAS */
	AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

	//NetUpdateFrequency = 100.f

	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(InPawn == nullptr)
		return;

	EnemyCharacter = Cast<AEnemyCharacter>(InPawn);
	
	if(EnemyCharacter)
	{
		if(EnemyCharacter->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(EnemyCharacter->GetBehaviorTree()->BlackboardAsset));
		}
	}

	GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddUniqueDynamic(this,&ThisClass::OnPerception);

	/*
	 * GAS
	 */
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, InPawn);
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
	ADemoCharacterBase* Player = Cast<ADemoCharacterBase>(GetPawn());
	if(Player)
	{
		PlayerCharacter=Player;
	}

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AEnemyController::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AEnemyController::MaxHealthChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AEnemyController::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &AEnemyController::MaxManaChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetLevelAttribute()).AddUObject(this, &AEnemyController::CharacterLevelChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemyController::StunTagChanged);
	}
}

void AEnemyController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	ADemoCharacterBase* Player = Cast<ADemoCharacterBase>(Actor);
	if(Player == nullptr)
		return;

	if(Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp,Warning,TEXT("Stimulus Success"));
		//BT Value Send True 
		Sight->PeripheralVisionAngleDegrees =180.f;

		GetBlackBoardComponent()->SetValueAsObject("Target",Actor);
		//테스트코드 -> 추후수정
		if(EnemyCharacter->GetMonsterType()!=EMonsterType::EMT_Attacking)
		{
			EnemyCharacter->SetBTMonsterType(EMonsterType::EMT_Move);
			SetFocus(Player);
		}
	}
	// else
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("Stimulus Fail"));
	// 	//BT Value Send False
	// 	Sight->PeripheralVisionAngleDegrees =75.f;
	// 	SetFocus(nullptr);
	// }
}

void AEnemyController::SetSensedTarget(APawn* NewTarget)
{
	ADemoCharacterBase* Player = Cast<ADemoCharacterBase>(NewTarget);
	if(BlackboardComponent)
	{
		if(NewTarget)
		{
			GetBlackBoardComponent()->SetValueAsObject("Target",Player);
			//테스트 코드 -> 추후수정
			if(EnemyCharacter->GetMonsterType()!=EMonsterType::EMT_Attacking)
			{
				SetFocus(NewTarget);
				EnemyCharacter->SetBTMonsterType(EMonsterType::EMT_Move);
			}
		}
	}
}

UAbilitySystemComponent* AEnemyController::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSetBase* AEnemyController::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AEnemyController::IsAlive() const
{
	return GetHealth() > 0.f;
}

void AEnemyController::ShowAbilityConfirmCancelText(bool ShowText)
{
}

float AEnemyController::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AEnemyController::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AEnemyController::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AEnemyController::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

int32 AEnemyController::GetCharacterLevel() const
{
	return AttributeSetBase->GetLevel();
}

void AEnemyController::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy HealthChanged"));
}

void AEnemyController::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy MaxHealthChanged"));
}

void AEnemyController::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy ManaChanged"));
}

void AEnemyController::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy MaxManaChanged"));
}

void AEnemyController::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy rLevelChanged"));
}

void AEnemyController::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCancelByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);

		UE_LOG(LogTemp, Warning, TEXT("Enemy StunTagChanged above 0"));

		BehaviorTreeComponent->StopTree();
		BehaviorTreeComponent->SetComponentTickEnabled(false);
	}else
	{
		BehaviorTreeComponent->StartTree(*EnemyCharacter->GetBehaviorTree(),EBTExecutionMode::Looped);
		BehaviorTreeComponent->SetComponentTickEnabled(true);
		UE_LOG(LogTemp, Warning, TEXT("Enemy StunTagChanged Below 0"));
	}
}

