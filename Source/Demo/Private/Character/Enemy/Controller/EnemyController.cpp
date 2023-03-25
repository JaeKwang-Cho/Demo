// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Controller/EnemyController.h"
#include "Character/Enemy/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Character/Player/DemoPlayerCharacterBase.h"
#include "Perception/AISenseConfig_Sight.h"


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
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
	ADemoCharacterBase* Player = Cast<ADemoCharacterBase>(GetPawn());
	if(Player)
	{
		PlayerCharacter=Player;
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

