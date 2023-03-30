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

UAnimMontage* UEnemyAnimInstance::GetDefaultAttackMontage()
{
	if(DefaultAttackMontage)
	{
		if(!Montage_IsPlaying(DefaultAttackMontage))
		{
			return DefaultAttackMontage;
		}
	}
	return nullptr;
}

UAnimMontage* UEnemyAnimInstance::GetDefaultThrowMontage()
{
	if(DefaultThrowMontage)
	{
		if(!Montage_IsPlaying(DefaultThrowMontage))
		{
			return DefaultThrowMontage;
		}
	}
	return nullptr;
}

void UEnemyAnimInstance::AnimNotify_DefaultThrow_GrabRock()
{
	Enemy->SpawnThrows();
}

void UEnemyAnimInstance::AnimNotify_DefaultThrow_ThrowRock()
{
	Enemy->LaunchThrows();
}
