// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoAssetManager.h"
#include "AbilitySystemGlobals.h"

void UDemoAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogTemp, Warning, TEXT("UAbilitySystemGlobals::Get().InitGlobalData()"));
}