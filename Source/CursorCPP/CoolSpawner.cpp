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

	// Spawn blocks in a grid pattern
	if (UWorld* World = GetWorld())
	{
		UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (!CubeMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load cube mesh"));
			return;
		}

		const int GridRows = 3;              // Number of rows in the grid
		const int GridCols = 4;              // Number of columns in the grid (3x4 = 12, but we'll only spawn 10)
		const float BoxSpacing = 24.0f;      // Space between each box (increased)
		const float BoxScale = 0.2f;         // Box scale
		const FVector StartLocation = FVector(0.0f, 0.0f, 0.0f); // Starting position

		int BlockCount = 0;
		int MaxBlocks = 10; // Only spawn 10 boxes

		for (int row = 0; row < GridRows && BlockCount < MaxBlocks; ++row)
		{
			for (int col = 0; col < GridCols && BlockCount < MaxBlocks; ++col)
			{
				// Calculate position in grid
				float X = StartLocation.X + (col * BoxSpacing);
				float Y = StartLocation.Y + (row * BoxSpacing);
				float Z = StartLocation.Z;
				
				// Spawn the box
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
		
		UE_LOG(LogTemp, Log, TEXT("Spawned %d boxes in grid pattern!"), BlockCount);
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

