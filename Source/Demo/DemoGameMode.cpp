// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoGameMode.h"
#include "DemoCharacter.h"
#include "Player/DemoPlayerState.h"
#include "UObject/ConstructorHelpers.h"

ADemoGameMode::ADemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	static ConstructorHelpers::FClassFinder<ADemoPlayerState> DemoPlayerState(TEXT("/Script/CoreUObject.Class'/Script/Demo.DemoPlayerState'"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		PlayerStateClass = DemoPlayerState.Class;
	}
}
