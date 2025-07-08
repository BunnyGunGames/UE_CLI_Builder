#include "CoolSpawner.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Json.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"

// Sets default values
ACoolSpawner::ACoolSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACoolSpawner::BeginPlay()
{
	Super::BeginPlay();

	// --- Enhanced Input Setup ---
	// Create Input Action
	LeftClickAction = NewObject<UInputAction>(this, TEXT("LeftClickAction"));
	LeftClickAction->ValueType = EInputActionValueType::Boolean;

	// Create Mapping Context
	MappingContext = NewObject<UInputMappingContext>(this, TEXT("CoolSpawnerMappingContext"));
	MappingContext->MapKey(LeftClickAction, EKeys::LeftMouseButton);

	// Add Mapping Context to Player
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
		// Bind the action
		if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			EnhancedInput->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ACoolSpawner::OnLeftMouseClick);
		}
	}

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
			MeshComp->SetMobility(EComponentMobility::Movable);
			MeshComp->SetStaticMesh(CubeMesh);
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
			
			// Create a looping animation cycle
			float CycleTime = AnimationDuration * 2.0f; // Full cycle (up + down)
			float CycleProgress = FMath::Fmod(AnimationTime, CycleTime) / CycleTime; // 0.0 to 1.0
			
			// Create a sine wave pattern for smooth up and down motion
			float SineValue = FMath::Sin(CycleProgress * 2.0f * PI);
			float EasedProgress = (SineValue + 1.0f) * 0.5f; // Convert from -1,1 to 0,1
			
			// Calculate new Z position
			float NewZ = BoxData.OriginalZ + (MaxHeightIncrease * EasedProgress);
			
			// Update actor position
			FVector CurrentLocation = BoxData.Actor->GetActorLocation();
			BoxData.Actor->SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, NewZ));
		}
	}
}

void ACoolSpawner::OnLeftMouseClick()
{
	SpawnRandomBlock();
}

void ACoolSpawner::SpawnRandomBlock()
{
	UWorld* World = GetWorld();
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!World || !CubeMesh) return;

	// Find the average position of existing blocks
	FVector Center(0, 0, 0);
	int32 Count = 0;
	for (const FBoxData& BoxData : SpawnedBoxes)
	{
		if (BoxData.Actor && BoxData.Actor->IsValidLowLevel())
		{
			Center += BoxData.Actor->GetActorLocation();
			++Count;
		}
	}
	if (Count > 0) Center /= Count;
	else Center = FVector(0, 0, 0);

	// Try to find a non-overlapping location
	FVector NewLocation;
	const float MinDistance = 120.0f; // Minimum distance from other blocks
	const int MaxTries = 20;
	bool bFound = false;
	for (int i = 0; i < MaxTries && !bFound; ++i)
	{
		float Angle = FMath::RandRange(0.0f, 2.0f * PI);
		float Radius = FMath::RandRange(180.0f, 300.0f);
		NewLocation = Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0);
		bFound = true;
		for (const FBoxData& BoxData : SpawnedBoxes)
		{
			if (BoxData.Actor && BoxData.Actor->IsValidLowLevel())
			{
				if (FVector::Dist2D(BoxData.Actor->GetActorLocation(), NewLocation) < MinDistance)
				{
					bFound = false;
					break;
				}
			}
		}
	}
	if (!bFound) return; // Could not find a spot

	// Spawn the new cube
	FRotator Rotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	AStaticMeshActor* CubeActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), NewLocation, Rotation, SpawnParams);
	if (CubeActor)
	{
		UStaticMeshComponent* MeshComp = CubeActor->GetStaticMeshComponent();
		MeshComp->SetMobility(EComponentMobility::Movable);
		MeshComp->SetStaticMesh(CubeMesh);
		CubeActor->SetActorScale3D(FVector(1.0f));
		if (BlockMaterial)
		{
			MeshComp->SetMaterial(0, BlockMaterial);
		}
		// Add to animation array, start animating immediately
		FBoxData BoxData;
		BoxData.Actor = CubeActor;
		BoxData.OriginalZ = NewLocation.Z;
		BoxData.StartTime = GetWorld()->GetTimeSeconds() - AnimationStartTime; // Start now
		BoxData.bIsAnimating = true;
		SpawnedBoxes.Add(BoxData);
	}
}

