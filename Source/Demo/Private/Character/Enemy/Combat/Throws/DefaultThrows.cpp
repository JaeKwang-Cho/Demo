// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Combat/Throws/DefaultThrows.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ADefaultThrows::ADefaultThrows()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = Body;
}

// Called when the game starts or when spawned
void ADefaultThrows::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADefaultThrows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

