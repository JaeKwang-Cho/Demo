// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Cores/BTVisitor.h"
#include "Character/Enemy/Monsters/EnemyPanchi.h"

void DescVisitor::Visit(AEnemyPanchi* Panchi)
{
	UE_LOG(LogTemp,Log,TEXT("I am Panchi"));
}

void AttackVisitor::Visit(AEnemyPanchi* Panchi)
{
	Panchi->DefaultAttack();
}

