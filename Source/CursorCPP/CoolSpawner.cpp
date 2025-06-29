#include "CoolSpawner.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
ACoolSpawner::ACoolSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACoolSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Timestamp log
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	UE_LOG(LogTemp, Log, TEXT("[%s] CoolSpawner BeginPlay called!!!"), *Timestamp);

	// Spawn blocks in a spiral pattern
	if (UWorld* World = GetWorld())
	{
		UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (!CubeMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load cube mesh"));
			return;
		}

		const int NumBlocks = 80;           // Number of blocks in the spiral staircase
		const float SpiralStartRadius = 20.0f; // Starting radius of the spiral
		const float SpiralRadiusStep = 6.0f;   // How much the radius increases per block
		const float AngleStepDeg = 12.0f;      // How much the angle increases per block (degrees)
		const float HeightStep = 4.0f;         // How much the height increases per block
		const float BoxScale = 0.25f;          // Box scale
		const FVector StartLocation = FVector(0.0f, 0.0f, 0.0f); // Center of spiral

		int BlockCount = 0;
		
		// Create spiral staircase formation
		for (int i = 0; i < NumBlocks; ++i)
		{
			float AngleRad = FMath::DegreesToRadians(i * AngleStepDeg);
			float Radius = SpiralStartRadius + i * SpiralRadiusStep;
			float X = StartLocation.X + FMath::Cos(AngleRad) * Radius;
			float Y = StartLocation.Y + FMath::Sin(AngleRad) * Radius;
			float Z = StartLocation.Z + i * HeightStep; // Staircase effect - each block is higher

			FVector Location(X, Y, Z);
			FRotator Rotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			AStaticMeshActor* BoxActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
			if (BoxActor)
			{
				UStaticMeshComponent* MeshComp = BoxActor->GetStaticMeshComponent();
				MeshComp->SetStaticMesh(CubeMesh);
				MeshComp->SetMobility(EComponentMobility::Movable);
				BoxActor->SetActorScale3D(FVector(BoxScale));

				// Apply the selected material if one is assigned
				if (BlockMaterial)
				{
					MeshComp->SetMaterial(0, BlockMaterial);
				}

				++BlockCount;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Spawned %d boxes in spiral staircase pattern!"), BlockCount);
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

