#include "CoolSpawner.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Json.h"

// Sets default values
ACoolSpawner::ACoolSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACoolSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Read Data/data.json for box data
	FString JsonFilePath = FPaths::ProjectDir() + TEXT("Data/data.json");
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load %s"), *JsonFilePath);
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON array from %s"), *JsonFilePath);
		return;
	}

	// Spawn cubes based on JSON data
	UWorld* World = GetWorld();
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!World || !CubeMesh) return;

	for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
	{
		if (!JsonValue->AsObject().IsValid())
		{
			continue;
		}

		TSharedPtr<FJsonObject> BoxObject = JsonValue->AsObject();
		
		// Get box properties
		int32 BoxNumber = BoxObject->GetIntegerField(TEXT("box"));
		float X = BoxObject->GetNumberField(TEXT("x"));
		float Y = BoxObject->GetNumberField(TEXT("y"));
		float Z = BoxObject->GetNumberField(TEXT("z"));

		// Spawn cube at specified coordinates
		FVector Location(X, Y, Z);
		FRotator Rotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		AStaticMeshActor* CubeActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
		if (CubeActor)
		{
			UStaticMeshComponent* MeshComp = CubeActor->GetStaticMeshComponent();
			MeshComp->SetStaticMesh(CubeMesh);
			MeshComp->SetMobility(EComponentMobility::Movable);
			CubeActor->SetActorScale3D(FVector(1.0f));
			
			// Apply material if available
			if (BlockMaterial)
			{
				MeshComp->SetMaterial(0, BlockMaterial);
			}
			
			UE_LOG(LogTemp, Log, TEXT("Spawned box %d at location (%f, %f, %f)"), BoxNumber, X, Y, Z);
		}
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

