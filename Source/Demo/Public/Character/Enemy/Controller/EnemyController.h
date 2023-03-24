// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(BlueprintReadWrite,Category = "AI Behavior",meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;
	
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
	
	
	
public:
	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent() const {return BlackboardComponent;}
};
