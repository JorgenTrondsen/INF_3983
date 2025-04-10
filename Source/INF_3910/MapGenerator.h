// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// #include "PCGGraph.h"
#include "MapGenerator.generated.h"


class UProceduralMeshComponent;



UCLASS()
class INF_3910_API AMapGenerator : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AMapGenerator();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;


public:	
	// Called after all components are initialized
    virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// function to enable replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Function to generate map
	UFUNCTION(BlueprintCallable, Category="Map Generation")
	void GenerateMap(int32 PlayerCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Generation")
	float BaseSize = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map Generation")
	float ScaleFactor = 100.f;
	

	UFUNCTION(BlueprintCallable, Category="Player Starts Generation")
	void GeneratePlayerStarts(int32 NumStarts, FVector Center, FVector Top, FVector Bottom, FVector Left, FVector Right);

	// UPROPERTY(EditAnywhere, Category="PCG Graph")
	// UPCGGraph* Graph = nullptr;

	// size of mesh
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int XSize = 100; // Number of squares along X axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int YSize = 100; // Number of squares along Y axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float ZMultiplier = 1000.0f; 

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float NoiseScale = 0.1f; 

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float Scale = 100.0f;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float UVScale = 1.0f;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int NumRings = 50;
	
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float BaseHeight = 1000.0f;
	


private:
	UPROPERTY(EditAnywhere, Category="Procedural Mesh Component")
	UProceduralMeshComponent* ProceduralMesh;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UV0;

	void CreateVertices();

	void CreateTriangles();

	UPROPERTY()
	bool bUseAsyncCooking = true;

	// replicated seed for map generation
	UPROPERTY(ReplicatedUsing=OnRep_MapSeed)
	int32 MapSeed;

	// add the option to use custom seed
	UPROPERTY(EditAnywhere, Category="Map Generation")
	bool bUseCustomSeed = true;

	UPROPERTY(EditAnywhere, Category="Map Generation", Meta = (EditCondition = "bUseCustomSeed"))
	int32 CustomSeed = 12345; // used if bUseCustomSeed is true

	// seed based noise generation, replaces the use of perlin noise
	float GenerateSeedBasedNoise(float X, float Y);

	// flag to track if we have generated the mesh
	bool bHasGeneratedMesh = false;

	// function for clients to generate the map upon receiving the seed
	UFUNCTION()
	void OnRep_MapSeed();

	// function to generate the mesh
	void GenerateMesh();

};
