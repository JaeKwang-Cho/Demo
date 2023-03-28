// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "../DemoCharacterBase.h"
#include "Cores/BTVisitor.h"
#include "PErception/PawnSensingComponent.h"

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "EnemyCharacter.generated.h"

class ADemoCharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDiedDelegate, AEnemyCharacter*, Character);

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	EMT_Idle	UMETA(DisplayName = "Idle"),	
	EMT_Move	UMETA(DisplayName = "Move"),	
	EMT_Circle	UMETA(DisplayName = "Circle"),
	EMT_Attack	UMETA(DisplayName = "Attack"),	
	EMT_Attacking	UMETA(DisplayName = "Attacking"),	
	EMT_Dead	UMETA(DisplayName = "Dead"),	
	
	EMT_Max		UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class EMonsterAttackRange : uint8
{
	EMAR_DefaultAttackRange	UMETA(DisplayName = "DefaultAttackRange"),	
	EMAR_CirclingRange_MAX UMETA(DisplayName = "CirclingRange_Max"),
	EMAR_CirclingRange_Min UMETA(DisplayName = "CirclingRange_Min"), 
	EMAR_Max		UMETA(DisplayName = "Max")
};

UCLASS()
class DEMO_API AEnemyCharacter : public ACharacter, public IAbilitySystemInterface
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
	/*
	 * GAS
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Demo|Enemy")
	FEnemyDiedDelegate OnEnemyDied;

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy")
	virtual int32 GetAbilityLevel(DemoAbilityID AbilityID) const;

	virtual void RemoveCharacterAbilities();

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy")
	virtual void FinishDying();

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Demo|Enemy|Attributes")
	float GetMaxMana() const;

protected:
	/*
	 * GAS
	 */
	TWeakObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Abilities")
	TArray<TSubclassOf<class UCharacterGameplayAbility>> CharacterAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	virtual void AddCharacterAbilities();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void SetHealth(float health);

	virtual void SetMana(float Mana);

	/*
	 *Anim
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Demo|Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Demo|Animation")
	class UAnimMontage* DeathMontage;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	
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

	//추후 DB�?뺄것 
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
