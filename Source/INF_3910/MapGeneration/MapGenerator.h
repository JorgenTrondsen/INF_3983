// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MapGenerator.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;
/**
 * Procedural map generator that creates terrain with a central plateau,
 * sloped mountains, and surrounding plains. Supports seeded generation
 * for deterministic multiplayer maps and automatic asset distribution.
*/
UCLASS()
class INF_3910_API AMapGenerator : public AActor
{
	GENERATED_BODY()

public:	
	// === CONSTRUCTOR & OVERRIDES ===
	AMapGenerator();
	virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// === MAIN API ===
	UFUNCTION(BlueprintCallable, Category="Map Generation")
	void GenerateMap();

	UFUNCTION(BlueprintCallable, Category="Player Starts Generation")
	void GeneratePlayerStarts(FVector Center, FVector Top, FVector Bottom, FVector Left, FVector Right);

	void SpawnAssets();

	// === TERRAIN GENERATION SETTINGS ===
	UPROPERTY(EditAnywhere, Category = "Terrain Generation", Meta = (ClampMin = 0))
	int XSize = 1000; // Number of squares along X axis

	UPROPERTY(EditAnywhere, Category = "Terrain Generation", Meta = (ClampMin = 0))
	int YSize = 1000; // Number of squares along Y axis

	UPROPERTY(EditAnywhere, Category = "Terrain Generation", Meta = (ClampMin = 0.000001))
	float Scale = 50.0f;
	
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float BaseHeight = 10000.0f;
	
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float ZMultiplier = 800.0f; 

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float NoiseScale = 0.02f; 
	
	// === SEED SETTINGS ===
	UPROPERTY(EditAnywhere, Category = "Terrain Generation")
	bool bUseCustomSeed = true;
	
	UPROPERTY(EditAnywhere, Category="Terrain Generation", Meta = (EditCondition = "bUseCustomSeed"))
	int32 CustomSeed = 12345; // used if bUseCustomSeed is true

	// === ASSET DISTRIBUTION ===
	UPROPERTY(EditAnywhere, Category = "Asset Distribution", meta = (ToolTip = "Rocks - spawn on all terrains"))
	TArray<TSubclassOf<AActor>> RockAssets;

	UPROPERTY(EditAnywhere, Category = "Asset Distribution", meta = (ToolTip = "Trees and bushes - spawn only on plains"))
	TArray<TSubclassOf<AActor>> VegetationAssets;


	UPROPERTY(EditAnywhere, Category = "Asset Distribution")
	TArray<TSubclassOf<AActor>> AssetClasses;
	
	UPROPERTY(EditAnywhere, Category = "Asset Distribution")
	int32 NumAssets = 2500;

	UPROPERTY(EditAnywhere, Category = "Asset Distribution")
	float MinAssetSpacing = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Asset Distribution")
	float AssetPlacementProb = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Asset Distribution")
	float MountainAssetDensity = 0.2f; // should be lower density on mountains

	UPROPERTY(EditAnywhere, Category = "Asset Distribution")
	float PlainsAssetDensity = 0.8f; // should be higher density on plains

	// === RENDERING ===
	UPROPERTY(EditAnywhere, Category = "Rendering")
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float UVScale = 0.1f;
	
private:
	// === COMPONENTS ===
	UPROPERTY(EditAnywhere, Category="Procedural Mesh Component")
	UProceduralMeshComponent* ProceduralMesh;

	// === MESH DATA ===
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UV0;
	TArray<FColor> Colors;

	// === COLOR FUNCTION ===
	uint8* GetRGB(float DistanceFromCenter, float PlateauRadius, float MountainRadius);

	// === STATE VARIABLES ===
	UPROPERTY(ReplicatedUsing=OnRep_MapSeed)
	int32 MapSeed;

	UPROPERTY()
	bool bUseAsyncCooking = true;
	
	bool bHasGeneratedMesh = false;
	
	// === CORE GENERATION FUNCTIONS ===
	void GenerateMesh();
	void CreateVertices();
	void CreateTriangles();

	// === TERRAIN CALCULATION FUNCTIONS ===
	UFUNCTION()
	void CalculateTerrainParameters(float& OutCenterX,
									float& OutCenterY,
									float& OutMaxRadius,
									float& OutPlateauRadius,
									float& OutMountainRadius,
									float& OutPlainsHeightFactor) const;
	
	UFUNCTION()
	float CalculateTerrainHeightAtPosition(float WorldX, float WorldY) const;

	// === NOISE GENERATION FUNCTIONS ===
	float GenerateSeedBasedNoise(float X, float Y) const;
	int32 HashCombine(int32 Seed, int32 Value) const;
	
	// === REPLICATION FUNCTIONS ===¨
	UFUNCTION()
	void OnRep_MapSeed();
	
	// === EXPERIMENTAL FUNCTIONS AND VARIABLES ===
	float SimpleNoise(float X, float Y);
	
	UPROPERTY(EditAnywhere, Category="Simple Noise")
	float SimpleFrequency = 0.02f;
	
	UPROPERTY(EditAnywhere, Category="Simple Noise")
	float SimpleAmplitude = 1000;
};
