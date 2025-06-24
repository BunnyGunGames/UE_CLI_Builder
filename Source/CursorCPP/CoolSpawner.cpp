// Fill out your copyright notice in the Description page of Project Settings.


#include "CoolSpawner.h"

// Sets default values
ACoolSpawner::ACoolSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACoolSpawner::BeginPlay()
{
	Super::BeginPlay();
FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
UE_LOG(LogTemp, Log, TEXT("[%s] CoolSpawner BeginPlay called"), *Timestamp);
	
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

