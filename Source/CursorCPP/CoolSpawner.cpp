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

	// Spawn blocks to form a pixel-art alpaca shape
	if (UWorld* World = GetWorld())
	{
		UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (!CubeMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load cube mesh"));
			return;
		}

		// Pixel-art alpaca (side view, 1 = block, 0 = empty)
		// Each row is Z (height), each col is X (width)
		TArray<TArray<int>> AlpacaGrid = {
			//  X: 0 1 2 3 4 5 6 7 8 9
			{0,0,0,0,1,1,0,0,0,0}, // Z: 11 (top of head/ears)
			{0,0,0,1,1,1,1,0,0,0}, // 10
			{0,0,0,1,1,1,1,0,0,0}, // 9
			{0,0,0,1,1,1,1,0,0,0}, // 8
			{0,0,0,1,1,1,1,0,0,0}, // 7 (head)
			{0,0,0,0,1,1,0,0,0,0}, // 6 (neck)
			{0,0,0,0,1,1,0,0,0,0}, // 5 (neck)
			{0,0,1,1,1,1,1,1,0,0}, // 4 (body)
			{0,1,1,1,1,1,1,1,1,0}, // 3 (body)
			{1,1,1,1,1,1,1,1,1,1}, // 2 (body)
			{0,0,1,0,0,0,0,1,0,0}, // 1 (legs)
			{0,0,1,0,0,0,0,1,0,0}, // 0 (feet)
		};

		const float BlockSize = 8.0f; // Small block size for detail
		const float Scale = 0.18f;    // Block scale for detail
		const int Rows = AlpacaGrid.Num();
		const int Cols = AlpacaGrid[0].Num();
		const float OriginX = -((Cols-1) * BlockSize) / 2.0f;
		const float OriginZ = 0.0f;

		int BlockCount = 0;
		for (int z = 0; z < Rows; ++z)
		{
			for (int x = 0; x < Cols; ++x)
			{
				if (AlpacaGrid[z][x] == 1)
				{
					FVector Location(OriginX + x * BlockSize, 0, OriginZ + (Rows-1-z) * BlockSize);
					FRotator Rotation = FRotator::ZeroRotator;
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					AStaticMeshActor* BoxActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
					if (BoxActor)
					{
						UStaticMeshComponent* MeshComp = BoxActor->GetStaticMeshComponent();
						MeshComp->SetStaticMesh(CubeMesh);
						MeshComp->SetMobility(EComponentMobility::Movable);
						BoxActor->SetActorScale3D(FVector(Scale));
						++BlockCount;
					}
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Spawned %d blocks for pixel-art alpaca!"), BlockCount);
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

