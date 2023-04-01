// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Combat/Throws/DefaultThrows.h"

#include "CollisionDebugDrawingPublic.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADefaultThrows::ADefaultThrows()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(50.f);
	SphereComponent->SetCollisionObjectType(ECC_GameTraceChannel2);
	SphereComponent->SetCollisionProfileName(TEXT("EnemyThrows"));
	this->SetLifeSpan(3.f);

	RootComponent = SphereComponent;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 0.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.1f;
}

// Called when the game starts or when spawned
void ADefaultThrows::BeginPlay()
{	
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Initial Speed is %f"), ProjectileMovementComponent->InitialSpeed);
}

// Called every frame
void ADefaultThrows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

