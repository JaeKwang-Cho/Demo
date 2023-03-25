// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "../DemoCharacterBase.h"
#include "Cores/BTVisitor.h"
#include "PErception/PawnSensingComponent.h"
#include "EnemyCharacter.generated.h"

class ADemoCharacterBase;

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	EMT_Idle	UMETA(DisplayName = "Idle"),	//플레이어 못찾았을떄 
	EMT_Move	UMETA(DisplayName = "Move"),	//플레이어 찾고 쫓아갈때 (전진, 후진, 경계..)
	EMT_Circle	UMETA(DisplayName = "Circle"),
	EMT_Attack	UMETA(DisplayName = "Attack"),	//공격할때 
	EMT_Attacking	UMETA(DisplayName = "Attacking"),	//공격할때 
	EMT_Dead	UMETA(DisplayName = "Dead"),	//HP 0일때
	
	EMT_Max		UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class EMonsterAttackRange : uint8
{
	EMAR_DefaultAttackRange	UMETA(DisplayName = "DefaultAttackRange"),	//무기상태
	EMAR_CirclingRange_MAX UMETA(DisplayName = "CirclingRange_Max"),
	EMAR_CirclingRange_Min UMETA(DisplayName = "CirclingRange_Min"), // 대치 상태
	EMAR_Max		UMETA(DisplayName = "Max")
};

UCLASS()
class DEMO_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AEnemyCharacter(const class FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);
	
	/*
	 *BT Visitor 
	 */
	UFUNCTION(BlueprintCallable)
	void ExecuteVisitor(FString key);

	virtual void Accept(VisitorPtr Visitor) {};
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();

	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void UIFindLookPlayer(class UWidgetComponent* _Widget);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(ADemoCharacterBase* _Target, float _AcceptanceRadius);

	UFUNCTION(BlueprintCallable)
	void MoveToCircleRange(ADemoCharacterBase* _Target);

	UFUNCTION(BlueprintCallable)
	void MoveInCircle(ADemoCharacterBase* _Target);

	float PlayHighPriorityMontage(UAnimMontage* Montage, FName StartSectionName, float InPlayRate);
	
protected:
	UFUNCTION(BlueprintCallable)
	virtual void TickAttackRangeCalculate();
	
	virtual void DefaultAttack();
	
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | BT",meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class AEnemyController* EnemyController;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum",meta = (AllowPrivateAccess = "true"))
	EMonsterType MonsterType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum",meta = (AllowPrivateAccess = "true"))
	EMonsterAttackRange MonsterAttackRange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combat | Distance",meta = (AllowPrivateAccess = "true"))
	float _Min_Circle = 300;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combat | Distance",meta = (AllowPrivateAccess = "true"))
	float _Max_Circle = 500;
	
	UPROPERTY()
	ADemoCharacterBase* Player;

	//DB
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Montage",meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DefaultAttackMontage;
	/*
	 *BT Visitor 
	 */
	std::map<FString,VisitorPtr> Visitors;

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Status",meta = (AllowPrivateAccess = "true"))
	float MaxHp;
    
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Status",meta = (AllowPrivateAccess = "true"))
    float Hp;
private:
	//HealthBar Timer
	FTimerHandle HealthBarTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | UITimer", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	//추후 DB로 뺄것 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | Status", meta = (AllowPrivateAccess = "true"))
	float DefaultAttackRange;

	//DB
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | Status", meta = (AllowPrivateAccess = "true"))
	float DefaultAttackPlayRate;




public:
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE void SetMaxHp(float _MaxHp) { MaxHp = _MaxHp; }

	FORCEINLINE float GetHp() const { return Hp; }
	FORCEINLINE void SetHp(float _Hp) { Hp = _Hp; }

	FORCEINLINE EMonsterType GetMonsterType() const {return MonsterType;}

	UFUNCTION(BlueprintCallable)
	void SetBTMonsterType(EMonsterType _Type);

	FORCEINLINE EMonsterAttackRange GetMonsterAttackRange() const {return MonsterAttackRange;}
	void SetBTMonsterAttackRange(EMonsterAttackRange _AttackRange);
};
