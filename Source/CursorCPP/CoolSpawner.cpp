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

	// Clear any existing boxes
	SpawnedBoxes.Empty();
	
	for (int32 i = 0; i < JsonArray.Num(); ++i)
	{
		const TSharedPtr<FJsonValue>& JsonValue = JsonArray[i];
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
			
			// Store box data for animation
			FBoxData BoxData;
			BoxData.Actor = CubeActor;
			BoxData.OriginalZ = Z;
			BoxData.StartTime = i * StaggerDelay; // Stagger the start times
			BoxData.bIsAnimating = false;
			SpawnedBoxes.Add(BoxData);
			
			UE_LOG(LogTemp, Log, TEXT("Spawned box %d at location (%f, %f, %f) with start time %f"), BoxNumber, X, Y, Z, BoxData.StartTime);
		}
	}
	
	// Start the animation timer
	AnimationStartTime = GetWorld()->GetTimeSeconds();
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Animate the boxes
	float CurrentTime = GetWorld()->GetTimeSeconds() - AnimationStartTime;
	
	for (FBoxData& BoxData : SpawnedBoxes)
	{
		if (!BoxData.Actor || !BoxData.Actor->IsValidLowLevel())
		{
			continue;
		}
		
		// Check if it's time to start animating this box
		if (CurrentTime >= BoxData.StartTime && !BoxData.bIsAnimating)
		{
			BoxData.bIsAnimating = true;
		}
		
		// Animate the box if it's in animation phase
		if (BoxData.bIsAnimating)
		{
			float AnimationTime = CurrentTime - BoxData.StartTime;
			float AnimationProgress = FMath::Clamp(AnimationTime / AnimationDuration, 0.0f, 1.0f);
			
			// Use a smooth easing function for the animation
			float EasedProgress = FMath::InterpEaseOut(0.0f, 1.0f, AnimationProgress, 2.0f);
			
			// Calculate new Z position
			float NewZ = BoxData.OriginalZ + (MaxHeightIncrease * EasedProgress);
			
			// Update actor position
			FVector CurrentLocation = BoxData.Actor->GetActorLocation();
			BoxData.Actor->SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, NewZ));
		}
	}
}

