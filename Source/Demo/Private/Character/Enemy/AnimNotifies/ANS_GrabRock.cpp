// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/AnimNotifies/ANS_GrabRock.h"
#include "Character/Enemy/EnemyCharacter.h"

void UANS_GrabRock::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	EnemyCharacter = Cast<AEnemyCharacter>(MeshComp->GetOwner());

	Hand_L_Location = MeshComp->GetSocketLocation(hand_l_Socket);
	Hand_R_Location = MeshComp->GetSocketLocation(hand_r_Socket);

	MiddleOfHands = (Hand_L_Location + Hand_R_Location) / 2.f;

	EnemyCharacter->SpawnThrows(MiddleOfHands);
}

void UANS_GrabRock::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	Hand_L_Location = MeshComp->GetSocketLocation(hand_l_Socket);
	Hand_R_Location = MeshComp->GetSocketLocation(hand_r_Socket);

	MiddleOfHands = (Hand_L_Location + Hand_R_Location) / 2.f;

	EnemyCharacter->SetThrowsLocation(MiddleOfHands);
}

FString UANS_GrabRock::GetNotifyName_Implementation() const
{
	return FString("GrabRock");
}
