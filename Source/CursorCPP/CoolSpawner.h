#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "CoolSpawner.generated.h"

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

public:	
	virtual void Tick(float DeltaTime) override;
};
