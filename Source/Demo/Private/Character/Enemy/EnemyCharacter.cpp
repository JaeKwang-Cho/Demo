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
#include "Character/Enemy/Combat/Throws/DefaultThrows.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Character/Enemy/EnemyAnimInstance.h"

#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"
#include "VectorTypes.h"
#include "Engine/Internal/Kismet/BlueprintTypeConversions.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer),
MonsterType(EMonsterType::EMT_Idle),
MonsterAttackRange(EMonsterAttackRange::EMAR_DefaultAttackRange),
MaxHp(100),
HealthBarDisplayTime(5.f),
DefaultAttackRange(200.f),
CirclingRange(400.f),
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

	if(GetMonsterType() == EMonsterType::EMT_CircleMove)
	{
		if(GetCircleType() == EMonster_CircleMove::ECM_KeepDistance)
		{
			TickKeepDistanceChecker();
		}
		if(GetCircleType() == EMonster_CircleMove::ECM_Circle)
		{
			TickCircleChecker();
		}
	}
	if(GetMonsterType() == EMonsterType::EMT_MeleeAttack)
	{
		if(GetMeleeType() == EMonster_MeleeAttack::EMA_Attacked)
		{
			TickBackStepChecker();
		}
		if(GetMeleeType() == EMonster_MeleeAttack::EMA_Draw_Near)
		{
			TickDrawNearChecker();
		}
	}	
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

	ThisCharacter = this;

	AnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
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

void AEnemyCharacter::DrawNearToTarget(float _AcceptanceRadius)
{
	if(EnemyController)
	{
		SetBTMeleeType(EMonster_MeleeAttack::EMA_Draw_Near);
		//UE_LOG(LogTemp, Warning, TEXT("SetBTMeleeType(EMonster_MeleeAttack::EMA_Draw_Near)"));
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Player);
		MoveRequest.SetAcceptanceRadius(_AcceptanceRadius);
		FNavPathSharedPtr NavPath;

		EnemyController->MoveToActor(Player, _AcceptanceRadius,true, true);
		//상태 스피드.. 등등..		
	}
}

void AEnemyCharacter::BackStepFromTarget(float _BackStepRange, float _AcceptanceRadius)
{
	if(EnemyController && Player)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SetBTMeleeType(EMonster_MeleeAttack::EMA_BackStep)"));
		
		FVector PlayerLocation = Player->GetActorLocation();
		FVector EnemyLocation = GetActorLocation();

		FVector Direction = EnemyLocation - PlayerLocation;
		Direction.Normalize(SMALL_NUMBER);
		Direction *= _BackStepRange;

		if(!bIsBackStepping)
		{
			bIsBackStepping = true;
			UE_LOG(LogTemp, Warning, TEXT("bIsBackStepping is True"));
			BackStepTargetLocation= EnemyLocation + Direction;
		}

		FAIMoveRequest aiMoveRequest(BackStepTargetLocation);
		aiMoveRequest.SetAcceptanceRadius(_AcceptanceRadius);
		aiMoveRequest.SetUsePathfinding(true);
		//FNavPathSharedPtr* OutPath;
		
		EnemyController->MoveTo(aiMoveRequest, nullptr);
		
	}
}

void AEnemyCharacter::MoveToCircleRange(float _AcceptanceRadius)
{
	if(EnemyController && !bIsMoveToCircleRange)
	{		
		SetBTCircleType(EMonster_CircleMove::ECM_KeepDistance);
		
		FVector PlayerLocation = Player->GetActorLocation();
		FVector EnemyLocation = GetActorLocation();

		FVector Direction = EnemyLocation - PlayerLocation;
		Direction.Normalize(SMALL_NUMBER);
		Direction *= _Min_Circle;

		CircleRangeTargetLocation = PlayerLocation + Direction;

		FAIMoveRequest aiMoveRequest(CircleRangeTargetLocation);
		aiMoveRequest.SetAcceptanceRadius(_AcceptanceRadius);
		aiMoveRequest.SetUsePathfinding(true);

		//FNavPathSharedPtr* OutPath;
		
		EnemyController->MoveTo(aiMoveRequest, nullptr);
		
		bIsMoveToCircleRange = true;
	}
}

void AEnemyCharacter::MoveInCircle(float _AcceptanceRadius, float SideStepUnit)
{
	if(EnemyController)
	{
		float MulForDirection;
		if(bIsRight)
		{
			MulForDirection = 1.f;
		}else
		{
			MulForDirection = -1.f;
		}
	
		FVector PlayerLocation = Player->GetActorLocation();
		FVector EnemyLocation = GetActorLocation();

		FVector DirectionToPlayer = PlayerLocation - EnemyLocation;
		FVector SideVector = FVector::CrossProduct(DirectionToPlayer, GetActorUpVector());
		SideVector.Normalize(SMALL_NUMBER);

		SideVector *= SideStepUnit;
		
		SideVector*= MulForDirection;

		CircleStepTargetLocation = GetActorLocation() + SideVector;

		FAIMoveRequest aiMoveRequest(CircleStepTargetLocation);
		aiMoveRequest.SetAcceptanceRadius(_AcceptanceRadius);
		aiMoveRequest.SetUsePathfinding(true);

		//FNavPathSharedPtr* OutPath;

		EnemyController->MoveTo(aiMoveRequest, nullptr);
	}
}

float AEnemyCharacter::PlayHighPriorityMontage(UAnimMontage* Montage, FName StartSectionName, float InPlayRate)
{
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(!AnimInstance->Montage_IsPlaying(Montage))
	{
		UAnimMontage* HighMontage = Montage;

		return PlayAnimMontage(HighMontage,InPlayRate,StartSectionName);
	}
	return 0.0f;
}

void AEnemyCharacter::TickDrawNearChecker()
{
	if(GetDistanceTo(Player) <= DefaultAttackRange)
	{
		SetBTMeleeType(EMonster_MeleeAttack::EMA_Attack);
		UE_LOG(LogTemp, Warning, TEXT("SetBTMeleeType(EMonster_MeleeAttack::EMA_Attack)"));
	}	
}

void AEnemyCharacter::TickKeepDistanceChecker()
{
	if(GetDistanceTo(Player) <= _Max_Circle)
	{
		bIsRight = FMath::RandBool();
		SetBTCircleType(EMonster_CircleMove::ECM_Circle);
		bIsMoveToCircleRange = false;
		UE_LOG(LogTemp, Warning, TEXT("SetBTCircleType(EMonster_CircleMove::ECM_Circling)"));
	}
}

void AEnemyCharacter::TickBackStepChecker()
{
	if(FVector::Dist(BackStepTargetLocation, GetActorLocation()) <= ToleranceDistance)
	{
		SetBTMeleeType(EMonster_MeleeAttack::EMA_BackStepped);
		bIsBackStepping = false;
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
		UE_LOG(LogTemp, Warning, TEXT("SetBTMeleeType(EMonster_MeleeAttack::EMA_BackStepped)"));
	}
	//UE_LOG(LogTemp, Warning, TEXT("Distance compare %.1f, %.1f"), FVector::Dist(BackStepTargetLocation, GetActorLocation()), ToleranceDistance);
}

void AEnemyCharacter::TickCircleChecker()
{
	if(GetDistanceTo(Player) > _Max_Circle)
	{
		SetBTCircleType(EMonster_CircleMove::ECM_KeepDistance);
		UE_LOG(LogTemp, Warning, TEXT("SetBTCircleType(EMonster_CircleMove::ECM_Approaching)"));
	}else if(GetDistanceTo(Player) <= DefaultAttackRange)
	{
		SetBTCircleType(EMonster_CircleMove::ECM_PlayerComeCloser);
		UE_LOG(LogTemp, Warning, TEXT("SetBTCircleType(EMonster_CircleMove::ECM_PlayerComeCloser)"));
	}
}

void  AEnemyCharacter::DefaultAttack()
{
	if(GetMonsterType()==EMonsterType::EMT_Dead) return;
	
	SetBTMeleeType(EMonster_MeleeAttack::EMA_Attacking);
	UE_LOG(LogTemp, Warning, TEXT("SetBTMeleeType(EMonster_MeleeAttack::EMA_Attacking)"));

	EnemyController->SetFocus(Player, EAIFocusPriority::Gameplay);
	
    //UE_LOG(LogTemp, Warning, TEXT("DefaultAttack()"));
	//PlayHighPriorityMontage(DefaultAttackMontage,FName("Attack"),DefaultAttackPlayRate);
	if(AnimInstance)
	{
		AnimInstance->PlayDefaultAttackMontage(DefaultAttackPlayRate);
	}
}

void AEnemyCharacter::CheckMeleeRangeCalculate()
{
	if(GetDistanceTo(Player) > DefaultAttackRange)
	{
		//거리 +70
		if(GetMeleeType() == EMonster_MeleeAttack::None)
		{
			SetBTMeleeType(EMonster_MeleeAttack::EMA_Draw_Near);
		}
	}else
	{
		if(GetMeleeType() == EMonster_MeleeAttack::EMA_Attacked)
		{
			SetBTMeleeType(EMonster_MeleeAttack::EMA_BackStep);
		}else
		{
			SetBTMeleeType(EMonster_MeleeAttack::EMA_Attack);	
		}
	}
}


void AEnemyCharacter::CheckCircleRangeCalculate()
{
	GetWorldTimerManager().SetTimer(CoolDownTimer, this, &AEnemyCharacter::OnCoolDown, 5.f, false);
	if(GetDistanceTo(Player) > _Max_Circle)
	{
		SetBTCircleType(EMonster_CircleMove::ECM_KeepDistance);
	}else if(GetDistanceTo(Player) >= _Min_Circle)
	{
		SetBTCircleType(EMonster_CircleMove::ECM_Circle);
	}else
	{
		SetBTCircleType(EMonster_CircleMove::ECM_PlayerComeCloser);
	}
}

void AEnemyCharacter::OnCoolDown()
{
	EnemyController->SetFocus(Player, EAIFocusPriority::Gameplay);
	
	ChangeMonsterTypeTo(EMonsterType::EMT_CircleMove, EMonster_CircleMove::ECM_SelectAttack, EMonster_MeleeAttack::None, EMonster_ThrowAttack::None);
	GetWorldTimerManager().ClearTimer(CoolDownTimer);
	UE_LOG(LogTemp,Warning,TEXT("CoolDown"));
}

/*
 * Throwing
 */

void  AEnemyCharacter::DefaultThrow()
{
	if(GetMonsterType()==EMonsterType::EMT_Dead) return;
		
	if(EnemyController)
		EnemyController->StopMovement();
	
	SetBTThrowType(EMonster_ThrowAttack::ETA_Throwing);
	
	//UE_LOG(LogTemp, Warning, TEXT("DefaultAttack()"));
	//PlayHighPriorityMontage(DefaultThrowMontage,FName("Throw"),DefaultAttackPlayRate);
	if(AnimInstance)
	{
		AnimInstance->PlayDefaultThrowMontage(DefaultAttackPlayRate);
	}
	EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
}

void AEnemyCharacter::SpawnThrows()
{
	FName hand_Two_Socket = FName("hand_Two_Socket");
	
	FTransform BetweenHandsLocation = GetMesh()->GetSocketTransform(hand_Two_Socket);

	//DrawDebugSphere(GetWorld(),BetweenHandsLocation.GetLocation(), 5.f, 8.f,FColor::Blue, false, 5.f,0,5 );

	DefaultThrows = GetWorld()->SpawnActor<ADefaultThrows>(DefaultThrowsClass.Get());
	//DefaultThrows->SphereComponent->SetCollisionProfileName(TEXT("NoCollision"));
	DefaultThrows->ProjectileMovementComponent->StopMovementImmediately();
	
	if(DefaultThrows)
	{
		FAttachmentTransformRules AttachRule(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
		DefaultThrows->AttachToComponent(GetMesh(), AttachRule, hand_Two_Socket);		
	}
}

void AEnemyCharacter::LaunchThrows()
{
	FDetachmentTransformRules DetachRule(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,true);
	DefaultThrows->DetachFromActor(DetachRule);

	DefaultThrows->Destroy();

	FName hand_Two_Socket = FName("hand_Two_Socket");
	FTransform BetweenHandsLocation = GetMesh()->GetSocketTransform(hand_Two_Socket);
	
	FVector OutLaunchVector;

	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutLaunchVector, BetweenHandsLocation.GetLocation(), Player->GetActorLocation());
	
	FRotator LookAtRotator = OutLaunchVector.Rotation();
	
	DefaultThrows = GetWorld()->SpawnActor<ADefaultThrows>(DefaultThrowsClass.Get(), BetweenHandsLocation.GetLocation(), LookAtRotator);
	DefaultThrows->ProjectileMovementComponent->Velocity = OutLaunchVector;
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

void AEnemyCharacter::SetBTMeleeType(EMonster_MeleeAttack _Type)
{
	if(EnemyController)
	{
		MeleeType = _Type;
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("MeleeType"),uint8(MeleeType));
	}
}

void AEnemyCharacter::SetBTThrowType(EMonster_ThrowAttack _Type)
{
	if(EnemyController)
	{
		ThrowType = _Type;
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ThrowType"),uint8(ThrowType));
	}
}

void AEnemyCharacter::SetBTCircleType(EMonster_CircleMove _Type)
{
	if(EnemyController)
	{
		CircleType = _Type;
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("CircleType"),uint8(CircleType));
	}
}

void AEnemyCharacter::ChangeMonsterTypeTo(EMonsterType _NextType, EMonster_CircleMove _NextCircle, EMonster_MeleeAttack _NextMelee, EMonster_ThrowAttack _NextThrow)
{
	SetBTCircleType(_NextCircle);
	SetBTMeleeType(_NextMelee);
	SetBTThrowType(_NextThrow);
	SetBTMonsterType(_NextType);
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


/*
 * GAS
 */
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


