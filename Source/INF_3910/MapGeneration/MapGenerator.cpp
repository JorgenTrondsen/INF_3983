// Fill out your copyright notice in the Description page of Project Settings.
#include "MapGenerator.h"
#include "CollisionQueryParams.h"
#include "Components/SceneComponent.h"
#include "Containers/Array.h"
#include "CoreGlobals.h"
#include "DrawDebugHelpers.h"
// #include "Elements/PCGAttributeNoise.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameEngine.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "HAL/Platform.h"
// #include "Helpers/PCGHelpers.h"
#include "LandscapeProxy.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"
#include "Landscape.h"
#include "Math/TransformCalculus2D.h"
#include "Math/UnrealMathUtility.h"
#include "ProceduralMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Net/UnrealNetwork.h"
#include "Templates/SubclassOf.h"
#include "Traits/IsContiguousContainer.h"
#include "UObject/CoreNet.h"
#include <cmath>
#include <cstdlib>
#include "INF_3910/Game/INFGameInstance.h"

void AMapGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
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

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, Colors, TArray<FProcMeshTangent>(), true);
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

	// calling generate map, which calls generate player starts
	// this is done such that the PIE player start is deleted before the mesh is generated
	GenerateMap();

	UE_LOG(LogTemp, Log, TEXT("MapGenerator: PostInitializeComponents - Generating Early"));
}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	bHasGeneratedMesh = false;

	// get game instance and check for a seed
	if (UGameInstance *GameInst = GetWorld()->GetGameInstance())
	{
		if (UINFGameInstance *MyGameInst = Cast<UINFGameInstance>(GameInst))
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
													 FString::Printf(TEXT("Generating map with this seed %d"), MapSeed));
				}

				GenerateMesh();
				SpawnAssets();
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,
												 35.0f,
												 FColor::Green,
												 FString::Printf(TEXT("could not cast game instance, seed:"), MapSeed));
			}
		}
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,
										 35.0f,
										 FColor::Green,
										 FString::Printf(TEXT("Generating map with this seed %d"), MapSeed));
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
											 FString::Printf(TEXT("OnRep_MapSeed: Calling GenerateMesh with seed"), MapSeed));
		}

		UE_LOG(LogTemp, Warning, TEXT("Client received MapSeed: %d"), MapSeed);
		// Always regenerate the mesh when we get a new seed
		bHasGeneratedMesh = false;
		GenerateMesh();
		// Assets are spawned by server and replicated, so don't spawn them on clients
	}
}

// Called every frame
void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapGenerator::GenerateMap()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,
										 15.0f,
										 FColor::Yellow,
										 FString::Printf(TEXT("Generating map based on 4 players")));
	}

	// // Calculate center point of our grid
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
	GeneratePlayerStarts(Center, Top, Bottom, Left, Right);

	if (bSpawnPOI)
	{
		SpawnPOI();
	}
}

void AMapGenerator::SpawnPOI()
{
	if (!POIClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No POI class selected in MapGenerator!"));
		return;
	}
	UWorld *World = GetWorld();
	if (!World)
		return;

	// Get terrain parameters
	float CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor;
	CalculateTerrainParameters(CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor);

	// Place POI at center of plateau
	float WorldX = CenterX - GetActorLocation().X;
	float WorldY = CenterY - GetActorLocation().Y;
	float TerrainHeight = CalculateTerrainHeightAtPosition(WorldX, WorldY);

	FVector POILocation = FVector(CenterX, CenterY, GetActorLocation().Z + TerrainHeight + 100.0f);

	// Spawn the POI
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	SpawnedPOI = World->SpawnActor<APOI>(POIClass, POILocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedPOI)
	{
		UE_LOG(LogTemp, Log, TEXT("POI spawned at plateau center: %s"), *POILocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn POI!"));
	}
}

float AMapGenerator::GenerateSeedBasedNoise(float X, float Y) const
{
	// Create a deterministic hash function based on coordinates and seed
	auto Hash = [this](int32 X, int32 Y) -> float
	{
		int32 Hash = MapSeed;
		Hash = HashCombine(Hash, X);
		Hash = HashCombine(Hash, Y);

		// Use random stream to get consistent values from hash
		FRandomStream Stream(Hash);
		return Stream.FRandRange(0.0f, 1.0f);
	};

	// Helper function for smooth interpolation
	auto SmoothStep = [](float T) -> float
	{
		return T * T * (3.0f - 2.0f * T);
	};

	// Create 2D value noise with bilinear interpolation
	auto ValueNoise2D = [&](float X, float Y) -> float
	{
		int32 X0 = FMath::FloorToInt(X);
		int32 Y0 = FMath::FloorToInt(Y);
		int32 X1 = X0 + 1;
		int32 Y1 = Y0 + 1;

		// Get fractional parts
		float Sx = SmoothStep(X - float(X0));
		float Sy = SmoothStep(Y - float(Y0));

		// Get random values for each corner
		float N00 = Hash(X0, Y0);
		float N01 = Hash(X0, Y1);
		float N10 = Hash(X1, Y0);
		float N11 = Hash(X1, Y1);

		// Bilinear interpolation
		float N0 = FMath::Lerp(N00, N10, Sx);
		float N1 = FMath::Lerp(N01, N11, Sx);
		float N = FMath::Lerp(N0, N1, Sy);

		// Map from [0,1] to [-1,1]
		return N * 2.0f - 1.0f;
	};

	// Generate fractal Brownian motion (multiple octaves)
	float Total = 0.0f;
	float Amplitude = 1.0f;
	float Frequency = 1.0f;
	float MaxValue = 0.0f;

	// Number of octaves controls detail level
	const int32 Octaves = 6;

	for (int32 i = 0; i < Octaves; i++)
	{
		// Get noise value for this octave
		float Value = ValueNoise2D(X * Frequency, Y * Frequency) * Amplitude;
		Total += Value;
		MaxValue += Amplitude;

		// Each octave has half amplitude and double frequency (roughness control)
		Amplitude *= 0.5f;
		Frequency *= 2.0f;
	}

	// Normalize to [-1,1] range
	return Total / MaxValue;
}

int32 AMapGenerator::HashCombine(int32 Seed, int32 Value) const
{
	return Seed ^ (Value + 0x9e3779b9 + (Seed << 6) + (Seed >> 2));
}

// method to generate the player starts and add them to the map
void AMapGenerator::GeneratePlayerStarts(FVector Center, FVector Top, FVector Bottom, FVector Left, FVector Right)
{

	// array to store all locations of player starts
	TArray<FVector> PlayerStarts;
	PlayerStarts.Add(Top);
	PlayerStarts.Add(Bottom);
	PlayerStarts.Add(Left);
	PlayerStarts.Add(Right);

	// get context of the map
	UWorld *World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null!"));
		return;
	}

	// Remove the existing PIE player starts before adding the created ones
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart *ExistingStart = *It;
		if (ExistingStart->GetName().Contains("PlayerStartPIE"))
		{
			ExistingStart->Destroy();
		}
	}
	// Create player starts actors
	int32 Index = 0;
	for (const FVector &Location : PlayerStarts)
	{
		// Calculate the exact terrain height at this position
		float WorldX = Location.X - GetActorLocation().X;
		float WorldY = Location.Y - GetActorLocation().Y;

		// Use the helper function to get exact terrain height
		float TerrainHeight = CalculateTerrainHeightAtPosition(WorldX, WorldY);

		// Set spawn location with safety margin above terrain
		FVector SpawnLocation = Location;
		float SafetyMargin = 300.0f; // Adjust as needed
		SpawnLocation.Z = GetActorLocation().Z + TerrainHeight + SafetyMargin;

		// Spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// Spawn the player start
		APlayerStart *NewPlayerStart = World->SpawnActor<APlayerStart>(
			APlayerStart::StaticClass(),
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams);

		if (NewPlayerStart)
		{
			// Name and tag the player start correctly
			FString PlayerStartName = FString::Printf(TEXT("CustomPlayerStart_%d"), Index);
			NewPlayerStart->SetActorLabel(PlayerStartName);

			// This is critical: Tag the player starts so GameMode can find them
			NewPlayerStart->PlayerStartTag = FName(TEXT("Player"));

			// Add a debug sphere to visualize the player start
			DrawDebugSphere(World, SpawnLocation, 50.0f, 16, FColor::Green, true, -1, 0, 2.0f);

			UE_LOG(LogTemp, Log, TEXT("Spawned %s at: %s (Terrain: %.2f)"),
				   *PlayerStartName, *SpawnLocation.ToString(), TerrainHeight);
			Index++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn PlayerStart at: %s"), *SpawnLocation.ToString());
		}
	}
}

void AMapGenerator::SpawnAssets()
{
	// Only spawn assets on the server - they will replicate to clients
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("SpawnAssets: Not spawning on client (no authority)"));
		return;
	}

	UWorld *World = GetWorld();
	if (!World)
		return;

	// Get terrain paramters
	float CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor;
	CalculateTerrainParameters(CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor);

	FRandomStream RandomStream(MapSeed);
	TArray<FVector> SpawnedLocations;

	for (int32 i = 0; i < NumAssets; i++)
	{
		// get random position within map bounds
		float X = RandomStream.FRandRange(0, XSize * Scale);
		float Y = RandomStream.FRandRange(0, YSize * Scale);

		// calculate distance from center
		float DeltaX = X - CenterX;
		float DeltaY = Y - CenterY;
		float DistanceFromCenter = FMath::Sqrt(DeltaX * DeltaX + DeltaY * DeltaY);

		// only spawn assets in the circluar map
		if (DistanceFromCenter > MaxRadius)
			continue;

		if (bSpawnPOI && SpawnedPOI)
		{
			float DistanceToPOI = FVector::Dist2D(FVector(X, Y, 0), SpawnedPOI->GetActorLocation());
			if (DistanceToPOI < POIClearanceRadius)
			{
				continue;
			}
		}

		// Apply different probabilities based on terrain (mountain or plains)
		TArray<TSubclassOf<AActor>> *AvailableAssets = nullptr;
		float PlacementProb = AssetPlacementProb;

		if (i < NumWeapons)
		{
			AvailableAssets = &WeaponAssets;
			PlacementProb = 1;
		}

		else if (DistanceFromCenter <= PlateauRadius)
		{
			AvailableAssets = &RockAssets; // Switch this with the building
			PlacementProb *= 0;
		}
		else if (DistanceFromCenter <= MountainRadius)
		{
			AvailableAssets = &RockAssets;
			PlacementProb *= MountainAssetDensity;
		}
		else
		{
			// Plains - randomy choose between rocks and vegetation
			if (RandomStream.RandRange(1, 10) <= 3)
			{
				AvailableAssets = &RockAssets;
			}
			else
			{
				AvailableAssets = &VegetationAssets;
			}

			PlacementProb *= PlainsAssetDensity;
		}

		// Skip if no assets available for this terrain
		if (!AvailableAssets || AvailableAssets->Num() == 0)
			continue;

		// Check placement probability
		if (RandomStream.FRand() > PlacementProb)
			continue;

		// Calculate spawn heigh, previously used linetracing
		float WorldX = X - GetActorLocation().X;
		float WorldY = Y - GetActorLocation().Y;
		float TerrainHeight = CalculateTerrainHeightAtPosition(WorldX, WorldY);
		FVector SpawnLocation = FVector(X, Y, GetActorLocation().Z + TerrainHeight);

		// Select random asset from available array
		int32 AssetIndex = RandomStream.RandRange(0, AvailableAssets->Num() - 1);
		TSubclassOf<AActor> AssetClass = (*AvailableAssets)[AssetIndex];

		// Spawn the asset
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		FRotator Rotation(0, RandomStream.FRandRange(0.0f, 360.0f), 0);

		float SizeFactor;
		// Rocks on the mountain should be smaller
		if (AvailableAssets == &RockAssets && DistanceFromCenter <= MountainRadius)
			SizeFactor = RandomStream.FRandRange(0.4f, 0.6f);
		else
			SizeFactor = RandomStream.FRandRange(0.8f, 1.2f);
		if (AActor *SpawnedAsset = World->SpawnActor<AActor>(AssetClass, SpawnLocation, Rotation, SpawnParams))
		{
			SpawnedAsset->SetActorScale3D(FVector(SizeFactor));

			// Ensure all spawned assets are replicated to clients
			SpawnedAsset->SetReplicates(true);

			SpawnedLocations.Add(SpawnLocation);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Spawned %d assets with seed %d"), SpawnedLocations.Num(), MapSeed);
}

float AMapGenerator::SimpleNoise(float X, float Y)
{

	// float frequency = 0.02f;
	// float amplitude = 1000;

	float XOffset = FMath::Sin(X * SimpleFrequency) * SimpleAmplitude;
	float YOffset = FMath::Sin(Y * SimpleFrequency) * SimpleAmplitude;

	// float Z = 100.0 + FMath::PerlinNoise2D(FVector2D(X, Y));
	float Z = 1.0;

	return Z;
}

void AMapGenerator::CalculateTerrainParameters(float &OutCenterX,
											   float &OutCenterY,
											   float &OutMaxRadius,
											   float &OutPlateauRadius,
											   float &OutMountainRadius,
											   float &OutPlainsHeightFactor) const
{
	// Calculate center point of our grid
	OutCenterX = XSize * Scale / 2.0f;
	OutCenterY = YSize * Scale / 2.0f;
	OutMaxRadius = FMath::Min(OutCenterX, OutCenterY);

	float MountainPeakHeight = BaseHeight;
	OutPlateauRadius = OutMaxRadius * 0.1f;
	OutMountainRadius = OutMaxRadius * 0.45f;

	// Calculate slope parameters
	const float SlopeAngleDegrees = 25.0f;
	const float SlopeAngleRadians = FMath::DegreesToRadians(SlopeAngleDegrees);
	float SlopeDistance = OutMountainRadius - OutPlateauRadius;
	float VerticalDrop = SlopeDistance * FMath::Tan(SlopeAngleRadians);
	float CalculatedPlainsHeight = MountainPeakHeight - VerticalDrop;
	OutPlainsHeightFactor = FMath::Max(CalculatedPlainsHeight / MountainPeakHeight, 0.1f);
}

float AMapGenerator::CalculateTerrainHeightAtPosition(float WorldX, float WorldY) const
{
	float CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor;
	CalculateTerrainParameters(CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor);

	// Calculate distance from center
	float DeltaX = WorldX - CenterX;
	float DeltaY = WorldY - CenterY;
	float DistanceFromCenter = FMath::Sqrt(DeltaX * DeltaX + DeltaY * DeltaY);

	float HeightFactor = 0.0f;
	float NoiseIntensity = 1.0f;

	// Your existing terrain zone logic
	if (DistanceFromCenter <= PlateauRadius)
	{
		HeightFactor = 1.0f;
		NoiseIntensity = 0.0f;
	}
	else if (DistanceFromCenter <= MountainRadius)
	{
		float CurrentSlopeDistance = DistanceFromCenter - PlateauRadius;
		float SlopeRange = MountainRadius - PlateauRadius;
		float SlopeProgress = CurrentSlopeDistance / SlopeRange;
		HeightFactor = FMath::Lerp(1.0f, PlainsHeightFactor, SlopeProgress);

		// Gradually introduce noise
		float NoiseTransitionDistance = PlateauRadius * 0.2f;
		if (CurrentSlopeDistance < NoiseTransitionDistance)
		{
			float NoiseProgress = CurrentSlopeDistance / NoiseTransitionDistance;
			NoiseIntensity = FMath::SmoothStep(0.0f, 1.0f, NoiseProgress);
		}
		else
		{
			NoiseIntensity = 1.0f;
		}
	}
	else
	{
		HeightFactor = PlainsHeightFactor;
		NoiseIntensity = 1.0f;
	}

	// Calculate noise at this position
	float NoiseX = WorldX / Scale;
	float NoiseY = WorldY / Scale;
	float NoiseHeight = GenerateSeedBasedNoise(NoiseX * NoiseScale, NoiseY * NoiseScale);

	// Return the calculated height
	return (BaseHeight * HeightFactor) + (NoiseHeight * ZMultiplier * NoiseIntensity * FMath::Max(HeightFactor, 0.2f));
}

uint8 *AMapGenerator::GetRGB(float DistanceFromCenter, float PlateauRadius, float MountainRadius)
{
	uint8 *RGB = (uint8 *)malloc(sizeof(uint8) * 3);

	float PlateauWeight = 0.0f;
	float SlopeWeight = 0.0f;
	float PlainsWeight = 0.0f;

	if (DistanceFromCenter <= PlateauRadius)
	{
		PlateauWeight = 1.0f;
	}
	else if (DistanceFromCenter <= MountainRadius)
	{
		float Progress = (DistanceFromCenter - PlateauRadius) / (MountainRadius - PlateauRadius);
		PlateauWeight = 1.0f - Progress;
		SlopeWeight = Progress;
	}
	else
	{
		float PlainsTransitionRadius = MountainRadius * 1.2f;
		if (DistanceFromCenter <= PlainsTransitionRadius)
		{
			float Progress = (DistanceFromCenter - MountainRadius) / (PlainsTransitionRadius - MountainRadius);
			SlopeWeight = 1.0f - Progress;
			PlainsWeight = Progress;
		}
		else
		{
			PlainsWeight = 1.0f;
		}
	}
	// Encode weights in vertex color (R=Plateau, G=Slope, B=Plains)
	RGB[0] = FMath::Clamp(PlateauWeight * 255.0f, 0.0f, 255.0f);
	RGB[1] = FMath::Clamp(SlopeWeight * 255.0f, 0.0f, 255.0f);
	RGB[2] = FMath::Clamp(PlainsWeight * 255.0f, 0.0f, 255.0f);

	return RGB;
}

void AMapGenerator::CreateVertices()
{
	Vertices.Empty();
	UV0.Empty();
	Colors.Empty(); // Add vertex colors

	// Get calculated parameters
	float CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor;
	CalculateTerrainParameters(CenterX, CenterY, MaxRadius, PlateauRadius, MountainRadius, PlainsHeightFactor);

	float MountainPeakHeight = BaseHeight;
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

			float Z;
			if (DistanceFromCenter <= MaxRadius)
			{
				Z = CalculateTerrainHeightAtPosition(WorldX, WorldY);
			}
			// Coordinates outside of the circle is set to a really low number
			else
			{
				Z = -5000.0f;
			}

			uint8 *RGB = GetRGB(DistanceFromCenter, PlateauRadius, MountainRadius);
			Colors.Add(FColor(RGB[0], RGB[1], RGB[2], 255));
			free(RGB);

			Vertices.Add(FVector(WorldX, WorldY, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

void AMapGenerator::CreateTriangles()
{
	Triangles.Empty();
	int Vertex = 0;
	for (int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y)
		{
			// Overside
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);

			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);

			++Vertex;
		}
		++Vertex;
	}
}