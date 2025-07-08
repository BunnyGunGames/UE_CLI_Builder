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

	// Read Data/data.json for box count
	FString JsonFilePath = FPaths::ProjectDir() + TEXT("Data/data.json");
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load %s"), *JsonFilePath);
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonArray) || JsonArray.Num() == 0 || !JsonArray[0]->AsObject().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON array from %s"), *JsonFilePath);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject = JsonArray[0]->AsObject();
	int32 BoxCount = JsonObject->GetIntegerField(TEXT("boxes"));

	// Spawn cubes in a row
	UWorld* World = GetWorld();
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!World || !CubeMesh) return;

	for (int32 i = 0; i < BoxCount; ++i)
	{
		FVector Location(i * 120.0f, 0.0f, 50.0f);
		FRotator Rotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		AStaticMeshActor* CubeActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
		if (CubeActor)
		{
			UStaticMeshComponent* MeshComp = CubeActor->GetStaticMeshComponent();
			MeshComp->SetStaticMesh(CubeMesh);
			MeshComp->SetMobility(EComponentMobility::Movable);
			CubeActor->SetActorScale3D(FVector(1.0f));
		}
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

