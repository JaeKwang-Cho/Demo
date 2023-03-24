// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Monsters/EnemyPanchi.h"

#include "Components/WidgetComponent.h"

AEnemyPanchi::AEnemyPanchi(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	//HealthBarWidget->SetupAttachment(GetRootComponent());
	
	Visitors.insert(std::make_pair("Desc",std::make_shared<DescVisitor>()));
	Visitors.insert(std::make_pair("DefaultAttack",std::make_shared<AttackVisitor>()));
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

void AEnemyPanchi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnemyPanchi::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEnemyPanchi::DefaultAttack()
{
	Super::DefaultAttack();
}


void AEnemyPanchi::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
	
}
