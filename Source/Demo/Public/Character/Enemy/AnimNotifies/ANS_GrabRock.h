// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_GrabRock.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UANS_GrabRock : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	class AEnemyCharacter* EnemyCharacter;

	const FName hand_l_Socket = FName("hand_l_Socket");
	const FName hand_r_Socket = FName("hand_r_Socket");
	
	FVector Hand_L_Location;
	FVector Hand_R_Location;

	FVector MiddleOfHands;
	
};
