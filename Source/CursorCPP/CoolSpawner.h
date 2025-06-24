#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoolSpawner.generated.h"

UCLASS()
class CURSORCPP_API ACoolSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoolSpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
