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

	// Spawn blocks in a square pattern
	if (UWorld* World = GetWorld())
	{
		UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (!CubeMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load cube mesh"));
			return;
		}

		const int SquareSize = 8;           // Size of the square (8x8 = 64 blocks)
		const float BlockSpacing = 30.0f;   // Space between each block
		const float BoxScale = 0.25f;       // Box scale
		const FVector StartLocation = FVector(0.0f, 0.0f, 0.0f); // Center of square

		int BlockCount = 0;
		
		// Calculate the starting position to center the square
		float StartX = StartLocation.X - (SquareSize - 1) * BlockSpacing * 0.5f;
		float StartY = StartLocation.Y - (SquareSize - 1) * BlockSpacing * 0.5f;
		
		// Create square formation
		for (int row = 0; row < SquareSize; ++row)
		{
			for (int col = 0; col < SquareSize; ++col)
			{
				float X = StartX + col * BlockSpacing;
				float Y = StartY + row * BlockSpacing;
				float Z = 0.0f;

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
		}
		UE_LOG(LogTemp, Log, TEXT("Spawned %d boxes in square pattern!"), BlockCount);
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

