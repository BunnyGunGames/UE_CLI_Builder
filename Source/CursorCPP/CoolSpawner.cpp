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
	UE_LOG(LogTemp, Log, TEXT("[%s] CoolSpawner BeginPlay called"), *Timestamp);

	// Spawn a visible StaticMeshActor using built-in sphere mesh
	if (UWorld* World = GetWorld())
	{
		FVector Location(0.f, 0.f, 0.f);
		FRotator Rotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// Spawn a StaticMeshActor
		AStaticMeshActor* SphereActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
		if (SphereActor)
		{
			UStaticMeshComponent* MeshComp = SphereActor->GetStaticMeshComponent();

			// Load the sphere mesh
			UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
			if (SphereMesh)
			{
				MeshComp->SetStaticMesh(SphereMesh);
				MeshComp->SetMobility(EComponentMobility::Movable);
				SphereActor->SetActorScale3D(FVector(1.0f));
				UE_LOG(LogTemp, Log, TEXT("Spawned sphere actor at (0,0,0)"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load sphere mesh"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn StaticMeshActor"));
		}
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
