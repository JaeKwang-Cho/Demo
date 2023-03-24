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
