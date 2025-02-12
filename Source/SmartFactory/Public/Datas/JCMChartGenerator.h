// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h" 
#include "JCMDataTypes.h"
#include "JCMChartGenerator.generated.h"
/**
 * 
 */

class UTextRenderComponent;

// ChartGenerator Abstract Component Class
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SMARTFACTORY_API UJCMChartGenerator : public USceneComponent
{
	GENERATED_BODY()

protected:

	// This Array contains Chart Base BP Actor 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TArray<UChildActorComponent*> ChildActorComponents;

	UFUNCTION(BlueprintCallable, Category = "Chart")
	virtual void ClearChildrenActors();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Chart")
	USceneComponent* ChildActorContainComponent;

public:
	UJCMChartGenerator();

	FChartGeneratingDoneDelegate ChartGeneratingDoneDelegate;
};

class USplineComponent;
class AJCMBarBaseActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UJCMChartGeneratorBar : public UJCMChartGenerator
{
	GENERATED_BODY()

private:
	// For Visually Modulation Bar Chart with Deviagtion 
	float CustomScaleValue = 1;
	float CustomPaddingScaleValue = 1;

	// Chart Value PreProcess function
	UFUNCTION()
	void PrepareBarValues(const TArray<float>& ValueArray, float& BarHeightScalerResult, float& BarPaddingResult,
		const float MaxHeight);

	// Create BarChart Function
	bool CreateBar(const TArray<float>& ValueArray, const TArray<FString>& LabelArray, const int BarSpacing,
		const float BarPaddingScaler, const float BarHeightScaler);

	// Update BarChart Data
	void UpdateBarChartData(const TArray<float>& ValueArray, const TArray<FString>& LabelArray);

	bool CreateBarAlongSplinePoint(const TArray<float>& ValueArray, const TArray<FString>& LabelArray,
		const float BarPaddingScaler, const float BarHeightScaler);

	void AddSplinePoint(USplineComponent* SplineComponent, int TargetCount);

	// BP BarBase Actor Source to Display 
	UPROPERTY()
	TSubclassOf<AJCMBarBaseActor> BarBaseActorBPClass;

	UPROPERTY()
	TArray<int32> SpawnedMeshAmounts;

protected:
	virtual void ClearChildrenActors() override;

public:
	UJCMChartGeneratorBar();

	// Generate Bar Chart Routine Function
	void GenerateBarChart(const FJCMChartDataShape& CopiedData, bool bGenerateMeshAtSplinePoint);

	// Set BP BarBase Actor Source to Display 
	void SetBarSourceActor(const TSubclassOf<AJCMBarBaseActor>& SourceActor);

	void SetAttachComponents(USceneComponent* TargetComponentInstance);

	UFUNCTION(BlueprintCallable, Category = "Component")
	const TArray<int32>& GetSpawnedMeshAmountArray() const
	{
		return SpawnedMeshAmounts;
	}

	UFUNCTION(BlueprintCallable, Category = "JCM")
	void ExecuteChartSearchingDelegate(const int32 InIndex);

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	USplineComponent* SplineComponent_length;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	USplineComponent* SplineComponent_height;

	FChartSearchingDelegate ChartSearchingDelegate;
};

UCLASS(Blueprintable)
class UJCMChartGeneratorLine : public UJCMChartGenerator
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class UJCMChartGeneratorPie : public UJCMChartGenerator
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class UJCMChartGeneratorXY : public UJCMChartGenerator
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class UJCMChartGeneratorXYZ : public UJCMChartGenerator
{
	GENERATED_BODY()

public:
};

