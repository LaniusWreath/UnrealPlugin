// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FactoryGizmo.generated.h"

UCLASS(Blueprintable)
class SMARTFACTORY_API AFactoryGizmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFactoryGizmo();


	// ----------------------- Override Event ----------------------------

	virtual void OnConstruction(const FTransform& Transform) override; 

	virtual bool ShouldTickIfViewportsOnly() const override;


	// ---------------------- Debug ------------------------

	UFUNCTION(BlueprintCallable, Category = "SFC|FactoryLayout")
	void ShowDebugArea();

	UFUNCTION(BlueprintCallable, Category = "SFC|FactoryLayout")
	void SetEnableDebugArea(bool inEnableDebugArea);


	// ----------------------- Generating Factory Modules ---------------------

	// Generating Area
	UFUNCTION(BlueprintCallable, Category = "SFC|FactoryLayout")
	void GenerateFactoryLayout();

	UFUNCTION(BlueprintCallable, Category = "SFC|FactoryLayout")
	void RemoveAndDestroyComponent();

	UStaticMesh* GetRandomMesh(const TArray<UStaticMesh*>& MeshArray);


public:

	// ----------------------- Factory Layout -------------------------------
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout")
	FVector StartPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout")
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout")
	int32 CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout")
	int32 FloorCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SFC|FactoryLayout")
	FVector Offset = FVector(10, 10, 13);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SFC|FactoryLayout")
	float UpperOffsetZ = 10;

	// ------------------------ Debug ---------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout", meta = (MakeEditWidget = "true"))
	bool bShowDebugArea;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout", meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMesh*> EdgeModules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout", meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMesh*> CornerModules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|FactoryLayout", meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMesh*> CenterModules;


private:

	// ----------------------- Generating Factory Modules ---------------------

	void SpawnEdgeAt(const FVector& InLocation, const FRotator& InRotation, const FVector& InScale);
	void SpawnCenterAt(const FVector& InLocation, const FVector& InScale);
	void SpawnCornerAt(const FVector& InLocation, const FRotator& InRotation, const FVector& InScale);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
