// Fill out your copyright notice in the Description page of Project Settings.
#include "MapGenerator.h"
#include "Components/SceneComponent.h"
#include "Containers/Array.h"
#include "CoreGlobals.h"
#include "DrawDebugHelpers.h"
#include "Elements/PCGAttributeNoise.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameEngine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "HAL/Platform.h"
#include "Helpers/PCGHelpers.h"
#include "LandscapeProxy.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Math/MathFwd.h"
#include "Landscape.h"
#include "Math/TransformCalculus2D.h"
#include "Math/UnrealMathUtility.h"
#include "ProceduralMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Net/UnrealNetwork.h"
#include "UObject/CoreNet.h"
#include <cstdlib>
#include "INF3910GameInstance.h"

void AMapGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicate the seed to all clients
	DOREPLIFETIME(AMapGenerator, MapSeed);
}

// Sets default values
AMapGenerator::AMapGenerator()
{
 	// do not call tick every frame
	PrimaryActorTick.bCanEverTick = false;

	// enable replication
	bReplicates = true;

	bHasGeneratedMesh = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(RootComponent);
	
	// settings for proper replication
	ProceduralMesh->SetEnableGravity(false);
	ProceduralMesh->SetCollisionProfileName("BlockAll");
	ProceduralMesh->bUseComplexAsSimpleCollision = true;

	bUseAsyncCooking = true;
}

void AMapGenerator::GenerateMesh()
{
	CreateVertices();
	CreateTriangles();

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
    ProceduralMesh->SetMaterial(0, Material);
    
    // Set up collision
    ProceduralMesh->ContainsPhysicsTriMeshData(true);
    if (ProceduralMesh->GetBodySetup())
    {
        ProceduralMesh->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseSimpleAndComplex;
    }
    
    bHasGeneratedMesh = true;
}

void AMapGenerator::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// // server initializes the seed
	// if(HasAuthority())
	// {
	// 	// generate random seed if custom seed is not enabled
	// 	if(!bUseCustomSeed)
	// 	{
	// 		MapSeed = FMath::Rand();
	// 	}
	// 	else 
	// 	{
	// 		MapSeed = CustomSeed;
	// 	}

	// 	// generate the mesh on the server	
	// 	// Generate the player starts on the server
	// }
	GenerateMap(4);

	// if (GEngine) 
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1,
	// 		35.0f,
	// 		FColor::Green,
	// 		FString::Printf(TEXT("PostInitializeComponents: Generating mesh with seed: %d "), MapSeed)
	// 	);
	// }
	
	// GenerateMesh();
	UE_LOG(LogTemp, Log, TEXT("MapGenerator: PostInitializeComponents - Generating Early"));

}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	bHasGeneratedMesh = false;

	// get game instance and check for a seed
	if (UGameInstance* GameInst = GetWorld()->GetGameInstance())
    {
        if (UINF3910GameInstance* MyGameInst = Cast<UINF3910GameInstance>(GameInst))
        {
            int32 StoredSeed = MyGameInst->GetMapSeed();
            
            // If we have a valid seed from the session
            if (StoredSeed != 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("Using seed from session: %d"), StoredSeed);
                
                // Override the replicated seed with our session seed
                MapSeed = StoredSeed;
                
                if (GEngine) 
				{
					GEngine->AddOnScreenDebugMessage(-1,
						35.0f,
						FColor::Green,
						FString::Printf(TEXT("Generating map with this seed %d"), MapSeed)
					);
				}

				GenerateMesh();
            }
        } else {
			if (GEngine) 
			{
				GEngine->AddOnScreenDebugMessage(-1,
					35.0f,
					FColor::Green,
					FString::Printf(TEXT("could not cast game instance, seed:"), MapSeed)
				);
			}
		}
    }
	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(-1,
			35.0f,
			FColor::Green,
			FString::Printf(TEXT("Generating map with this seed %d"), MapSeed)
		);
	}

    // Just log mesh status
    bool HasMeshData = (Vertices.Num() > 0);
    bool HasCollision = ProceduralMesh->GetBodySetup() != nullptr;
    
    UE_LOG(LogTemp, Warning, TEXT("MapGenerator BeginPlay - HasMeshData: %d, HasCollision: %d, VertexCount: %d"),
           HasMeshData, HasCollision, Vertices.Num());
}

void AMapGenerator::OnRep_MapSeed()
{
	if (!HasAuthority())
	{
		if (GEngine) 
			{
				GEngine->AddOnScreenDebugMessage(-1,
					35.0f,
					FColor::Green,
					FString::Printf(TEXT("OnRep_MapSeed: Calling GenerateMesh with seed"), MapSeed)
				);
			}

		UE_LOG(LogTemp, Warning, TEXT("Client received MapSeed: %d"), MapSeed);
        // Always regenerate the mesh when we get a new seed
        bHasGeneratedMesh = false;
		GenerateMesh();
	}
}

// Called every frame
void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapGenerator::GenerateMap(int32 PlayerCount)
{
	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(-1,
			15.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Generating map based on %d players"), PlayerCount)
		);
	}

	// Calculate center point of our grid
	float CenterX = XSize * Scale / 2.0f;
	float CenterY = YSize * Scale / 2.0f;
	
	
	// Calculate maximum radius (use whichever is smaller to ensure circle fits within rectangle)
    float MaxRadius = FMath::Min(CenterX, CenterY);
    
    // get the center of the map using the actor's location
    FVector Center;
	Center.X = CenterX;
	Center.Y = CenterY;
    Center.Z = 1000.f; // Set the Z to 0 or another appropriate height
    
    // calculate the spawn locations for the player starts
    float SpawnOffset = MaxRadius * 0.8f; // Place players at 80% of the radius
    FVector Top = Center + FVector(0, SpawnOffset, 0);
    FVector Bottom = Center + FVector(0, -SpawnOffset, 0);
    FVector Left = Center + FVector(-SpawnOffset, 0, 0);
    FVector Right = Center + FVector(SpawnOffset, 0, 0);
    
    // generate the player starts
    GeneratePlayerStarts(PlayerCount, Center, Top, Bottom, Left, Right);

}

float AMapGenerator::GenerateSeedBasedNoise(float X, float Y)
{
	// initialize a random stream with the seed and the X,Y position
	// this ensures the same coordinates will always return the same value
	int32 PositionSeed = MapSeed + (X * 1000) + (Y * 1000);


	if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("GenerateSeedBasedNoise: Creating noise with seed: %d"), MapSeed)
			);
		}


	FRandomStream RandomStream(PositionSeed);

	// deterministic noise algorithm
	// multiple octaves of noise to create a more natural look

	float Amplitude = 1.0f;
	float Frequency = 1.0f;
	float NoiseValue = 0.0f;
	float TotalAmplitude = 0.0f;

	for (int i = 0; i < 4; i++)
	{
		float PointValue = 0.0f;
		for (int j = 0; j < 4; j++)
		{
			PointValue += (RandomStream.FRandRange(0.0f, 1.0f) * 2.0f - 1.0f);
		}
		PointValue /= 4.0f;

		NoiseValue += PointValue * Amplitude;
		TotalAmplitude += Amplitude;

		Amplitude *= 0.5f;
		Frequency *= 2.0f;
	}

	return NoiseValue / TotalAmplitude;
}

// method to generate the player starts and add them to the map
void AMapGenerator::GeneratePlayerStarts(int32 NumStarts, FVector Center, FVector Top, FVector Bottom, FVector Left, FVector Right)
{
	
	// array to store all locations of player starts
	TArray<FVector> PlayerStarts;
	
	// add all positions of the playerstarts to the array
	if (NumStarts == 1)
	{
		PlayerStarts.Add(Center);
	}
	else if (NumStarts == 2)
	{
		PlayerStarts.Add(Top);
		PlayerStarts.Add(Bottom);	
	}
	else if (NumStarts == 3)
	{
		PlayerStarts.Add(Bottom);
		PlayerStarts.Add(Left);
		PlayerStarts.Add(Right);
	}
	else 
	{
		PlayerStarts.Add(Top);
		PlayerStarts.Add(Bottom);
		PlayerStarts.Add(Left);
		PlayerStarts.Add(Right);
	}
	
	// get context of the map
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null!"));
		return;
	}

	// remove the existing PIE player starts to use the ones we generate
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* ExistingStart = *It;
		if (ExistingStart->GetName().Contains("PlayerStartPIE"))
		{
			ExistingStart->Destroy();
			UE_LOG(LogTemp, Log, TEXT("Removed existing PIE PlayerStart: %s"), *ExistingStart->GetName());
		}
	}

	// create player starts actors
	int32 Index = 0;
	for (const FVector& Location : PlayerStarts)
	{
		// find the proper height for the player start by tracing down
		FVector StartPos = Location;
		StartPos.Z = 1000.0f; // start trace from above

		FVector EndPos = Location;
		EndPos.Z = -1000.0f; // end trace below

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		// adjust the Z position based on terrain height
		FVector SpawnLocation = Location;
		
		// trace to find ground position
		if (World->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECC_Visibility, QueryParams))
		{
			SpawnLocation.Z = HitResult.Location.Z + 100.f; // add some offset to avoid clipping
		}
		else
		{
			// this is used in case the line trace fails

			// Fallback - use terrain height formula to calculate an approximate height
			float DeltaX = Location.X - GetActorLocation().X - XSize * Scale / 2.0f;
			float DeltaY = Location.Y - GetActorLocation().Y - YSize * Scale / 2.0f;
			float DistanceFromCenter = FMath::Sqrt(DeltaX * DeltaX + DeltaY * DeltaY);
			float MaxRadius = FMath::Min(XSize, YSize) * Scale / 2.0f;
			float MountainRadius = MaxRadius * 0.4f;
			float PlainsStartHeight = 0.2f;
			
			// Calculate rough height using same formula as terrain
			float HeightFactor = 0.0f;
			if (DistanceFromCenter <= MountainRadius)
			{
				float MountainNormalizedDistance = DistanceFromCenter / MountainRadius;
				HeightFactor = FMath::Pow(1.0f - MountainNormalizedDistance, 3.0f);
				HeightFactor = FMath::Max(HeightFactor, PlainsStartHeight);
			}
			else
			{
				float PlainDistance = (DistanceFromCenter - MountainRadius) / (MaxRadius - MountainRadius);
				HeightFactor = PlainsStartHeight * (1.0f - PlainDistance);
			}
			
			// Apply the height
			SpawnLocation.Z = GetActorLocation().Z + (BaseHeight * HeightFactor) + 100.0f;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// spawn the player start
		APlayerStart* NewPlayerStart = World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

		if (NewPlayerStart)
		{
			// Name and tag the player start correctly
			FString PlayerStartName = FString::Printf(TEXT("CustomPlayerStart_%d"), Index);
            NewPlayerStart->SetActorLabel(PlayerStartName);
		
			// This is critical: Tag the player starts so GameMode can find them
            NewPlayerStart->PlayerStartTag = FName(TEXT("Player"));
            
            // Add a debug sphere to visualize the player start
            DrawDebugSphere(World, SpawnLocation, 50.0f, 16, FColor::Green, true, -1, 0, 2.0f);
            
            UE_LOG(LogTemp, Log, TEXT("Spawned %s at: %s"), *PlayerStartName, *SpawnLocation.ToString());
            Index++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn PlayerStart at: %s"), *SpawnLocation.ToString());
		}

	}


	// // loop through the array and create player start spawn actors
	// for (const FVector& Location : PlayerStarts)
	// {
	// 	FActorSpawnParameters SpawnParams;
	// 	SpawnParams.Owner = this;

	// 	// spawn the player start at the computed locationh
	// 	APlayerStart* NewPlayerStart = World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
	// 	if (NewPlayerStart)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("Spawned PlayerStart at: %s"), *Location.ToString());
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn PlayerStart at: %s"), *Location.ToString());
	// 	}
//	}
}

void AMapGenerator::CreateVertices()
{

	Vertices.Empty();
    UV0.Empty();

    // Calculate center point of our grid
    float CenterX = XSize * Scale / 2.0f;
    float CenterY = YSize * Scale / 2.0f;
    
    // Calculate maximum radius (use whichever is smaller to ensure circle fits within rectangle)
    float MaxRadius = FMath::Min(CenterX, CenterY);


	// parameters for mountain shape
    float MountainPeakHeight = BaseHeight;  // Height at the very center
    float MountainExponent = 3.0f;         // Controls steepness (higher = steeper peak)
    float MountainRadius = MaxRadius * 0.4f; // Where the mountain base ends and plains begin

	float PlainsStartHeight = 0.2f;

	// width of the transition area between mountain and plains
	float TransitionWidth = MountainRadius * 0.1f;

    // Generate the rectangular grid
    for (int X = 0; X <= XSize; ++X)
    {
        for (int Y = 0; Y <= YSize; ++Y)
        {
            // Position in world space
            float WorldX = X * Scale;
            float WorldY = Y * Scale;
            
            // Calculate distance from center
            float DeltaX = WorldX - CenterX;
            float DeltaY = WorldY - CenterY;
            float DistanceFromCenter = FMath::Sqrt(DeltaX * DeltaX + DeltaY * DeltaY);

			float NormalizedDistance = FMath::Min(DistanceFromCenter / MaxRadius, 1.0f);

			// Calculate height factor:
            float HeightFactor = 0.0f;

			if (DistanceFromCenter <= MountainRadius - TransitionWidth)
			{
				// inside the mountain area
				float MountainNormalizedDistance = DistanceFromCenter / MountainRadius;
				// Power function for steep dropoff
				HeightFactor = FMath::Pow(1.0f - MountainNormalizedDistance, MountainExponent);

				HeightFactor = FMath::Max(HeightFactor, PlainsStartHeight);
			}
			else if (DistanceFromCenter <= MountainRadius + TransitionWidth)
			{
				// In the transition zone - blend between mountain and plains
                float TransitionFactor = (DistanceFromCenter - (MountainRadius - TransitionWidth)) / (2.0f * TransitionWidth);
                TransitionFactor = FMath::Clamp(TransitionFactor, 0.0f, 1.0f);
                
                // Calculate both heights and blend between them
                float MountainHeight = PlainsStartHeight; // Use the minimum height from mountain
                
                float PlainDistance = (DistanceFromCenter - MountainRadius) / (MaxRadius - MountainRadius);
                float PlainHeight = PlainsStartHeight * (1.0f - PlainDistance);
                
                // Smoothly blend using a cosine interpolation for extra smoothness
                HeightFactor = FMath::Lerp(MountainHeight, PlainHeight, TransitionFactor);
			}

			else
			{
				// outside mountain area - gentle plains
				float PlainDistance = (DistanceFromCenter - MountainRadius) / (MaxRadius - MountainRadius);
				// small heihgt for plains that gradually decreases to edge
				HeightFactor = PlainsStartHeight * (1.0f - PlainDistance);
			}

			float NoiseHeight = GenerateSeedBasedNoise(X * NoiseScale, Y * NoiseScale);

			// Combine the height factor with the noise and apply the multiplier
			// float BaseHeight = 1000.0f;
			float Z = (MountainPeakHeight * HeightFactor) + (NoiseHeight * ZMultiplier * FMath::Max(HeightFactor, 0.2f));


            // Apply circular mask:
            // 1. If inside the circle, use the calculated height
            // 2. If outside but close, create a slope downward (optional)
            // 3. If far outside, set very low (effectively removing from view)
            float Falloff = 50.0f; // Width of the edge slope
            if (DistanceFromCenter > MaxRadius)
            {
                float DistanceOutside = DistanceFromCenter - MaxRadius;
                
                if (DistanceOutside < Falloff)
                {
                    // Create a slope at the edge
                    float EdgeFactor = DistanceOutside / Falloff;
                    Z = Z * (1.0f - EdgeFactor) - 500.0f * EdgeFactor;
                }
                else
                {
                    // Set vertices far outside the circle to a very low position
                    Z = -5000.0f;
                }
            }
            
            Vertices.Add(FVector(WorldX, WorldY, Z));
            UV0.Add(FVector2D(X * UVScale, Y * UVScale));
            
            // Debug output for a few vertices only
            // if (X % 10 == 0 && Y % 10 == 0)
            // {
            //     GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, 
            //         FString::Printf(TEXT("Vertex (%d,%d): Z=%f, Distance=%f"), X, Y, Z, DistanceFromCenter));
            // }
        }
    }
	// ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	

	// for rectangular mesh
	// for (int X = 0; X <= XSize; ++X)
	// {
	// 	for (int Y = 0; Y <= YSize; ++Y)
	// 	{
	// 		float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;
	// 		GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z: %f"), Z));
	// 		Vertices.Add(FVector(X * Scale, Y * Scale, Z));
	// 		UV0.Add(FVector2D(X * UVScale, Y * UVScale));

	// 		// DrawDebugSphere(GetWorld(), FVector(X * Scale, Y * Scale, 0), 25.0f, 16, FColor::Red, true, -1.0f, 0U, 0.0f);
	// 	}
	// }
}

void AMapGenerator::CreateTriangles()
{
    Triangles.Empty();

	// for rectangular mesh
	int Vertex = 0;

	for (int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y)
		{
			// Overside
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			
			// Underside 
			// Triangles.Add(Vertex);
			// Triangles.Add(Vertex + YSize + 1);
			// Triangles.Add(Vertex + 1);

			// Overside
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);
		
			// Underside
			// Triangles.Add(Vertex + 1);
			// Triangles.Add(Vertex + YSize + 1);
			// Triangles.Add(Vertex + YSize + 2);
		
			++Vertex;
		}
		++Vertex;
	}

}