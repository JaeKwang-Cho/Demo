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
	
	EMT_Chase	UMETA(DisplayName = "Chase"),
	
	EMT_CircleMove	UMETA(DisplayName = "CircleMove"),
	
	EMT_MeleeAttack	UMETA(DisplayName = "MeleeAttack"),

	EMT_ThrowAttack UMETA(DisplayName = "ThrowAttack"),
	
	EMT_Dead	UMETA(DisplayName = "Dead"),	
	
	EMT_Max		UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class EMonster_MeleeAttack : uint8
{
	None UMETA(DisplayName = "None"),
	
	EMA_Draw_Near UMETA(DisplayName = "Draw_Near"),
	
	EMA_Attack	UMETA(DisplayName = "Attack"),	
	EMA_Attacking	UMETA(DisplayName = "Attacking"),
	EMA_Attacked	UMETA(DisplayName = "Attacked"),
	
	EMA_BackStep	UMETA(DisplayName = "BackStep"),
	EMA_BackStepped UMETA(DisplayName = "BackStepped"),
};

UENUM(BlueprintType)
enum class EMonster_ThrowAttack : uint8
{
	None UMETA(DisplayName = "None"),
	
	ETA_Throw	UMETA(DisplayName = "Throw"),
	ETA_Throwing	UMETA(DisplayName = "Throwing"),	
	ETA_Thrown	UMETA(DisplayName = "Thrown"),	
};

UENUM(BlueprintType)
enum class EMonster_CircleMove : uint8
{
	None UMETA(DisplayName = "None"),
	
	ECM_KeepDistance	UMETA(DisplayName = "KeepDistance"),

	ECM_Circle		UMETA(DisplayName = "Circle"),

	ECM_PlayerComeCloser	UMETA(DisplayName = "PlayerComeCloser"),
	//ECM_AttackedForCounter UMETA(DisplayName = "AttackedForCounter"),

	ECM_SelectAttack	UMETA(DisplayName = "SelectAttack"),
};

UENUM(BlueprintType)
enum class EMonsterAttackRange : uint8
{
	EMAR_DefaultAttackRange	UMETA(DisplayName = "DefaultAttackRange"),	
	EMAR_CirclingRange UMETA(DisplayName = "CirclingRange"),
	EMAR_Max		UMETA(DisplayName = "Max")
};

UCLASS()
class DEMO_API AEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
private:
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
	UPROPERTY()
	class UEnemyAnimInstance* AnimInstance;

public:
	UEnemyAnimInstance* GetAnimInstance() const
	{
		return AnimInstance;
	}

protected:
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

	/*
	 *  AI - Action
	 */

	// Melee
	UFUNCTION(BlueprintCallable)
	void DrawNearToTarget(float _AcceptanceRadius =50.f);

	UFUNCTION(BlueprintCallable)
	void BackStepFromTarget(float _BackStepRange = 200.f, float _AcceptanceRadius = 50.f);
	bool bIsBackStepping = false;
	
	UFUNCTION(BlueprintCallable)
	virtual void  DefaultAttack();

	UFUNCTION(BlueprintCallable)
	virtual void CheckMeleeRangeCalculate();

	// Circle
	UFUNCTION(BlueprintCallable)
	void MoveToCircleRange(float _AcceptanceRadius = 50.f);
	bool bIsRight = true;
	bool bIsMoveToCircleRange = false;
	FVector CircleRangeTargetLocation;

	UFUNCTION(BlueprintCallable)
	void MoveInCircle(float _AcceptanceRadius = 50.f, 	float SideStepUnit = 100.f);
	FVector CircleStepTargetLocation;

	UFUNCTION(BlueprintCallable)
	virtual void CheckCircleRangeCalculate();

	// Throw
	
	UFUNCTION(BlueprintCallable)
	virtual void  DefaultThrow();

	float PlayHighPriorityMontage(UAnimMontage* Montage, FName StartSectionName, float InPlayRate);

	/*
	 *  AI - Checker
	 */
	virtual void TickDrawNearChecker();

	virtual void TickKeepDistanceChecker();
	
	virtual void TickBackStepChecker();
	FVector BackStepTargetLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BehaviorTree", meta = (AllowPrivateAccess = "true"))
	float ToleranceDistance = 100.f;
	
	virtual void TickCircleChecker();

public:
	/*
	 *  AI - AnimNotify Callback
	 */
	virtual void SpawnThrows();

	virtual void LaunchThrows();
	
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | BT",meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class AEnemyController* EnemyController;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum",meta = (AllowPrivateAccess = "true"))
	EMonsterType MonsterType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum | Sub",meta = (AllowPrivateAccess = "true"))
	EMonster_MeleeAttack MeleeType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum | Sub",meta = (AllowPrivateAccess = "true"))
	EMonster_CircleMove CircleType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum | Sub",meta = (AllowPrivateAccess = "true"))
	EMonster_ThrowAttack ThrowType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Enum",meta = (AllowPrivateAccess = "true"))
	EMonsterAttackRange MonsterAttackRange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combat | Distance",meta = (AllowPrivateAccess = "true"))
	float _Min_Circle = 400;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combat | Distance",meta = (AllowPrivateAccess = "true"))
	float _Max_Circle = 700;
	
	UPROPERTY()
	ADemoCharacterBase* Player;

	//DB

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Parent | Montage",meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ADefaultThrows> DefaultThrowsClass;

	UPROPERTY()
	ADefaultThrows* DefaultThrows;
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
	FTimerHandle CoolDownTimer;

	void OnCoolDown();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | UITimer", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | Status", meta = (AllowPrivateAccess = "true"))
	float DefaultAttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | Status", meta = (AllowPrivateAccess = "true"))
	float CirclingRange;

	//DB
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Parent | Status", meta = (AllowPrivateAccess = "true"))
	float DefaultAttackPlayRate;

public:
	float GetDefaultAttackPlayRate() const
	{
		return DefaultAttackPlayRate;
	}

private:
	UPROPERTY()
	AEnemyCharacter* ThisCharacter;

public:
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE void SetMaxHp(float _MaxHp) { MaxHp = _MaxHp; }

	FORCEINLINE float GetHp() const { return Hp; }
	FORCEINLINE void SetHp(float _Hp) { Hp = _Hp; }

	FORCEINLINE EMonsterType GetMonsterType() const {return MonsterType;}

	FORCEINLINE EMonster_MeleeAttack GetMeleeType() const {return MeleeType;}

	FORCEINLINE EMonster_CircleMove GetCircleType() const	{return CircleType;}

	FORCEINLINE EMonster_ThrowAttack GetThrowType() const	{return ThrowType;}

	UFUNCTION(BlueprintCallable)
	void SetBTMonsterType(EMonsterType _Type);

	UFUNCTION(BlueprintCallable)
	void SetBTMeleeType(EMonster_MeleeAttack _Type);
	
	UFUNCTION(BlueprintCallable)
	void SetBTThrowType(EMonster_ThrowAttack _Type);

	UFUNCTION(BlueprintCallable)
	void SetBTCircleType(EMonster_CircleMove _Type);

	void ResetBelowTypes(){
		MeleeType = EMonster_MeleeAttack::None;
		CircleType = EMonster_CircleMove::None;
		ThrowType = EMonster_ThrowAttack::None;
	}

	UFUNCTION(BlueprintCallable)
	void ChangeMonsterTypeTo(EMonsterType _NextType,
		EMonster_CircleMove _NextCircle = EMonster_CircleMove::None,
		EMonster_MeleeAttack _NextMelee = EMonster_MeleeAttack::None,
		EMonster_ThrowAttack _NextThrow = EMonster_ThrowAttack::None);
	

	FORCEINLINE EMonsterAttackRange GetMonsterAttackRange() const {return MonsterAttackRange;}
	void SetBTMonsterAttackRange(EMonsterAttackRange _AttackRange);

public:
	/*
	 * Temp
	 */
	UPROPERTY(EditAnywhere,Category = "Temp")
	float VecMul = 100;
};
