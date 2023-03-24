// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DemoPlayerController.h"
#include "Player/DemoPlayerState.h"
#include "AbilitySystemComponent.h"

void ADemoPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ADemoPlayerState* PS = GetPlayerState<ADemoPlayerState>();

	if (PS)
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}