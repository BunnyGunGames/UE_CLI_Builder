#include "CoolSpawner.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

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

	// Spawn a physics ball and floor
	if (UWorld* World = GetWorld())
	{
		// Load sphere mesh for the ball
		UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (!SphereMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load sphere mesh"));
			return;
		}

		// Load plane mesh for the floor
		UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
		if (!PlaneMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load plane mesh"));
			return;
		}

		// Spawn the floor first
		FVector FloorLocation = FVector(0.0f, 0.0f, -50.0f); // Below the ball
		FRotator FloorRotation = FRotator::ZeroRotator;
		FActorSpawnParameters FloorSpawnParams;
		FloorSpawnParams.Owner = this;

		AStaticMeshActor* FloorActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FloorLocation, FloorRotation, FloorSpawnParams);
		if (FloorActor)
		{
			UStaticMeshComponent* FloorMeshComp = FloorActor->GetStaticMeshComponent();
			FloorMeshComp->SetStaticMesh(PlaneMesh);
			FloorMeshComp->SetMobility(EComponentMobility::Static);
			FloorActor->SetActorScale3D(FVector(20.0f, 20.0f, 1.0f)); // Large floor
			
			// Enable physics on the floor
			FloorMeshComp->SetSimulatePhysics(false); // Floor doesn't move
			FloorMeshComp->SetEnableGravity(false);
			FloorMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			
			UE_LOG(LogTemp, Log, TEXT("Spawned floor at location: %s"), *FloorLocation.ToString());
		}

		// Spawn the physics ball
		FVector BallLocation = FVector(0.0f, 0.0f, 200.0f); // High above the floor
		FRotator BallRotation = FRotator::ZeroRotator;
		FActorSpawnParameters BallSpawnParams;
		BallSpawnParams.Owner = this;

		AStaticMeshActor* BallActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), BallLocation, BallRotation, BallSpawnParams);
		if (BallActor)
		{
			UStaticMeshComponent* BallMeshComp = BallActor->GetStaticMeshComponent();
			BallMeshComp->SetStaticMesh(SphereMesh);
			BallMeshComp->SetMobility(EComponentMobility::Movable);
			BallActor->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)); // Normal size ball
			
			// Enable physics on the ball
			BallMeshComp->SetSimulatePhysics(true);
			BallMeshComp->SetEnableGravity(true);
			BallMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			
			// Create a bouncy physics material
			UPhysicalMaterial* BouncyMaterial = NewObject<UPhysicalMaterial>();
			if (BouncyMaterial)
			{
				BouncyMaterial->Restitution = 1.0f;  // Very bouncy (0.0 = no bounce, 1.0 = perfect bounce)
				BouncyMaterial->Friction = 0.1f;      // Low friction for more sliding/bouncing
				BouncyMaterial->FrictionCombineMode = EFrictionCombineMode::Average;
				
				// Apply the bouncy material to the ball
				BallMeshComp->SetPhysMaterialOverride(BouncyMaterial);
			}
			
			// Apply the selected material if one is assigned
			if (BlockMaterial)
			{
				BallMeshComp->SetMaterial(0, BlockMaterial);
			}
			
			UE_LOG(LogTemp, Log, TEXT("Spawned bouncy physics ball at location: %s"), *BallLocation.ToString());
		}
	}
}

// Called every frame
void ACoolSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

