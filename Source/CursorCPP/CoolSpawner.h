#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMeshActor.h"
#include "CoolSpawner.generated.h"

USTRUCT()
struct FBoxData
{
	GENERATED_BODY()

	UPROPERTY()
	AStaticMeshActor* Actor = nullptr;
	
	UPROPERTY()
	float StartTime = 0.0f;
	
	UPROPERTY()
	float OriginalZ = 0.0f;
	
	UPROPERTY()
	bool bIsAnimating = false;
};

UCLASS()
class CURSORCPP_API ACoolSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoolSpawner();

protected:
	virtual void BeginPlay() override;

	// Material to apply to the spawned blocks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spiral Staircase", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* BlockMaterial;

	// Animation properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AnimationDuration = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float StaggerDelay = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float MaxHeightIncrease = 200.0f;

private:
	// Array to track spawned boxes and their animation data
	UPROPERTY()
	TArray<FBoxData> SpawnedBoxes;
	
	// Timer to track when animation started
	float AnimationStartTime;

public:	
	virtual void Tick(float DeltaTime) override;
};
