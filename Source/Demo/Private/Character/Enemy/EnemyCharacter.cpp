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
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer),
MonsterType(EMonsterType::EMT_Idle),
MonsterAttackRange(EMonsterAttackRange::EMAR_DefaultAttackRange),
MaxHp(100),
HealthBarDisplayTime(5.f),
DefaultAttackRange(300.f),
DefulatAttackPlayRate(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	
}

void AEnemyCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	AEnemyController* _Controller = Cast<AEnemyController>(GetController());

	if(_Controller && PawnInstigator!=this)
	{
		_Controller->SetSensedTarget(PawnInstigator);
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
	EnemyController = Cast<AEnemyController>(GetController());
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
	if(GetMonsterType()!=EMonsterType::EMT_Attacking)
		TickAttackRangeCalculate();

	
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

		EnemyController->MoveToActor(_Target, _AcceptanceRadius,true, false);
		//상태 스피드.. 등등..

		
	}
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
	
	switch (MonsterAttackRange)
	{
	case EMonsterAttackRange::EMAR_DefaultAttackRange:

		if(GetDistanceTo(Player) <= DefaultAttackRange)
		{
			//거리 +70
			SetBTMonsterType(EMonsterType::EMT_Attack);
		}
	default:
		;
	}
}

void AEnemyCharacter::DefaultAttack()
{
	if(GetMonsterType()==EMonsterType::EMT_Dead) return;
	
	SetBTMonsterType(EMonsterType::EMT_Attacking);

	PlayHighPriorityMontage(DefaultAttackMontage,FName("Attack"),DefulatAttackPlayRate);

	if(EnemyController)
		EnemyController->StopMovement();
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


