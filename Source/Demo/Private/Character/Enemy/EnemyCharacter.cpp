// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/Enemy/Controller/EnemyController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/Player/DemoPlayerCharacterBase.h"
//#include "Character/Player/Camera/BOTWPlayerCameraManager.h"
#include "Interfaces/ITargetDevice.h"
#include "Components/CapsuleComponent.h"

#include "Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/AbilitySystem/CharacterGameplayAbility.h"

#include "Animation/AnimMontage.h"
#include "GameplayEffect.h"
#include "UObject/UObjectGlobals.h"
#include "GameplayAbilitySpec.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer),
MonsterType(EMonsterType::EMT_Idle),
MonsterAttackRange(EMonsterAttackRange::EMAR_DefaultAttackRange),
MaxHp(100),
HealthBarDisplayTime(5.f),
DefaultAttackRange(200.f),
DefaultAttackPlayRate(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));
}

void AEnemyCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	if(EnemyController && PawnInstigator!=this)
	{
		EnemyController->SetSensedTarget(PawnInstigator);
	}
}

void AEnemyCharacter::AddCharacterAbilities()
{
	if (!AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UCharacterGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(
				StartupAbility, 
				GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID),
				static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID),
				this
			)
		);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AEnemyCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("%s() AbilitySystemComponent isn't valid for %s"), *FString(__FUNCTION__), *GetName());
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttirbutes for %s, Please fill in the character's Blueprint"), *FString(__FUNCTION__), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);

	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("%s() MakeOutgoingSpec isn't working"), *FString(__FUNCTION__), *GetName());
	}
}

void AEnemyCharacter::AddStartupEffects()
{
	if (!AbilitySystemComponent.IsValid() || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);

		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void AEnemyCharacter::SetHealth(float health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(health);
	}
}

void AEnemyCharacter::SetMana(float Mana)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetMana(Mana);
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Restart ConstructionScripts
	RerunConstructionScripts();

	//Have a Controller on Spawn
	SpawnDefaultController();

	//Get AIController
	//EnemyController = Cast<AEnemyController>(GetController());
	if(EnemyController)
	{
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
	if(PawnSensingComponent)
	{
		PawnSensingComponent->OnHearNoise.AddUniqueDynamic(this,&ThisClass::OnHearNoise);
	}
	//Test
	ShowHealthBar();

	Player = Cast<ADemoCharacterBase>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if(EMonsterType::EMT_Attack == MonsterType || EMonsterType::EMT_Move == MonsterType)
	{
		TickAttackRangeCalculate();
	}
	*/
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EnemyController = Cast<AEnemyController>(NewController);
	UE_LOG(LogTemp, Warning, TEXT("Get AI Controller = EnemyController"));
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//Executed when an actor is deleted
	if(EnemyController)
	{
		EnemyController->UnPossess();
		EnemyController->Destroy();
	}
}

void AEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//Enemy Basic Settings
	//DataTable & Dynamic Materials
	
	SetHp(GetMaxHp());
}

void AEnemyCharacter::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ThisClass::HideHealthBar, HealthBarDisplayTime);
}

void AEnemyCharacter::UIFindLookPlayer(UWidgetComponent* _Widget)
{
	if(_Widget)
	{
		auto PlayerCamera =  UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
		
		if(PlayerCamera)
		{
			const FVector Start = _Widget->GetComponentLocation();
			const FVector End = PlayerCamera->GetCameraLocation();
			_Widget->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(FVector(Start.X,Start.Y,0.f),
				FVector(End.X,End.Y,0.f)));
		}
	}
}

void AEnemyCharacter::MoveToTarget(ADemoCharacterBase* _Target, float _AcceptanceRadius)
{
	if(EnemyController)
	{
		SetBTMonsterType(EMonsterType::EMT_Move);
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(_Target);
		MoveRequest.SetAcceptanceRadius(_AcceptanceRadius);
		FNavPathSharedPtr NavPath;

		EnemyController->MoveToActor(_Target, _AcceptanceRadius,true, true);
		//상태 스피드.. 등등..		
	}
}

void AEnemyCharacter::MoveToCircleRange(ADemoCharacterBase* _Target)
{
	if(EnemyController)
	{
		SetBTMonsterType(EMonsterType::EMT_Move);
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(_Target);
		
		const float Distance = FMath::RandRange(_Min_Circle, _Max_Circle);
		MoveRequest.SetAcceptanceRadius(Distance);
		FNavPathSharedPtr NavPath;

		EnemyController->MoveToActor(_Target, Distance,true, true);
		//상태 스피드.. 등등..		
	}
}

void AEnemyCharacter::MoveInCircle(ADemoCharacterBase* _Target)
{
	
}

float AEnemyCharacter::PlayHighPriorityMontage(UAnimMontage* Montage, FName StartSectionName, float InPlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(!AnimInstance->Montage_IsPlaying(Montage))
	{
		UAnimMontage* HighMontage = Montage;

		return PlayAnimMontage(HighMontage,InPlayRate,StartSectionName);
	}
	return 0.0f;
}

void AEnemyCharacter::TickAttackRangeCalculate()
{
	float Distance  = GetDistanceTo(Player);
	if(Distance < 200.f)
	{
		SetBTMonsterType(EMonsterType::EMT_Attack);
	}else if(Distance < 600.f)
	{
		//SetBTMonsterType(EMonsterType::EMT_Circle);
	}else
	{
		SetBTMonsterType(EMonsterType::EMT_Move);
	}
}

void AEnemyCharacter::DefaultAttack()
{
	if(GetMonsterType()==EMonsterType::EMT_Dead) return;
	
	//SetBTMonsterType(EMonsterType::EMT_Attacking);

    //UE_LOG(LogTemp, Warning, TEXT("DefaultAttack()"));
	PlayHighPriorityMontage(DefaultAttackMontage,FName("Attack"),DefaultAttackPlayRate);
}

/*
 *
 *BTType Setting
 * 
 */
void AEnemyCharacter::SetBTMonsterType(EMonsterType _Type)
{
	if(EnemyController)
	{
		MonsterType = _Type;
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("MonsterType"),uint8(MonsterType));
	}
}

void AEnemyCharacter::SetBTMonsterAttackRange(EMonsterAttackRange _AttackRange)
{
	if(EnemyController)
	{
		MonsterAttackRange = _AttackRange;
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("TypeAttackRange"),uint8(MonsterAttackRange));
	}
}

/*
 * 
 *BT Visitor
 * 
 */

void AEnemyCharacter::ExecuteVisitor(FString key)
{
	auto it = Visitors.find(key);
	if(it != Visitors.end())
		Accept(it->second);
}

UAbilitySystemComponent* AEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool AEnemyCharacter::IsAlive() const
{
	return GetHealth() > 0.f;
}

int32 AEnemyCharacter::GetAbilityLevel(DemoAbilityID AbilityID) const
{
	return 1;
}

void AEnemyCharacter::RemoveCharacterAbilities()
{
	if (!AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void AEnemyCharacter::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnEnemyDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->CancelAbilities();

		FGameplayTagContainer EffectsTagsToRemove;
		EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AEnemyCharacter::FinishDying()
{
	Destroy();
}

int32 AEnemyCharacter::GetCharacterLevel() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetLevel();
	}

	return 0;
}

float AEnemyCharacter::GetHealth() const
{
	if(AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.f;
}

float AEnemyCharacter::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.f;
}

float AEnemyCharacter::GetMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMana();
	}

	return 0.f;
}

float AEnemyCharacter::GetMaxMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxMana();
	}

	return 0.f;
}


