// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyAnimInstance.h"
#include "Character/Enemy/EnemyCharacter.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Enemy = Cast<AEnemyCharacter>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(Enemy)
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.f;

		Speed = Velocity.Size();

		// Direction = CalculateDirection(Velocity, Enemy->GetActorRotation());
		// bIsInAir = Enemy->GetMovementComponent()->IsFalling();
	}
	
}

void UEnemyAnimInstance::PlayDefaultAttackMontage(float PlayRate)
{
	if(DefaultAttackMontage)
	{
		if(!Montage_IsPlaying(DefaultAttackMontage))
		{
			Montage_Play(DefaultAttackMontage, PlayRate);
		}
	}
}

void UEnemyAnimInstance::PlayDefaultThrowMontage(float PlayRate)
{
	if(DefaultThrowMontage)
	{
		if(!Montage_IsPlaying(DefaultThrowMontage))
		{
			Montage_Play(DefaultThrowMontage, PlayRate);
		}
	}
}

void UEnemyAnimInstance::AnimNotify_DefaultThrow_GrabRock() const
{
	Enemy->SpawnThrows();
}

void UEnemyAnimInstance::AnimNotify_DefaultThrow_ThrowRock() const
{
	Enemy->LaunchThrows();
}

void UEnemyAnimInstance::AnimNotify_AttackEnd()
{
	Enemy->SetBTMeleeType(EMonster_MeleeAttack::EMA_Attacked);
}

void UEnemyAnimInstance::AnimNotify_ThrowEnd()
{
	Enemy->SetBTThrowType(EMonster_ThrowAttack::ETA_Thrown);
}
