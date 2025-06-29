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

		const int BlocksPerDiagonal = 11; // Number of blocks per diagonal (center shared)
		const float XLength = 240.0f;    // Length of the X arms
		const float BoxScale = 0.23f;    // Box scale
		const FVector StartLocation = FVector(0.0f, 0.0f, 0.0f); // Center of X

		int BlockCount = 0;
		// First diagonal: bottom-left to top-right
		for (int i = 0; i < BlocksPerDiagonal; ++i)
		{
			float t = (float)i / (BlocksPerDiagonal - 1); // 0 to 1
			float X = StartLocation.X + (t - 0.5f) * XLength;
			float Y = StartLocation.Y + (t - 0.5f) * XLength;
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
		// Second diagonal: top-left to bottom-right (skip center block to avoid overlap)
		for (int i = 0; i < BlocksPerDiagonal; ++i)
		{
			if (i == BlocksPerDiagonal / 2) continue; // Skip center
			float t = (float)i / (BlocksPerDiagonal - 1); // 0 to 1
			float X = StartLocation.X + (t - 0.5f) * XLength;
			float Y = StartLocation.Y - (t - 0.5f) * XLength;
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
		UE_LOG(LogTemp, Log, TEXT("Spawned %d boxes in X pattern!"), BlockCount);
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

