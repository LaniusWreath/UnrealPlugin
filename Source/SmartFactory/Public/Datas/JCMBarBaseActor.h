// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JCMDataTypes.h"
#include "JCMBarBaseActor.generated.h"

class UTextRenderComponent;
class UTimelineComponent;
class UProceduralMeshComponent;

UCLASS()
class SMARTFACTORY_API AJCMBarBaseActor : public AActor
{
	GENERATED_BODY()

private:

protected:

	int32 ParentSplineIndex;

	UPROPERTY()
	UTimelineComponent* BarAnimationTimeline;

	UPROPERTY(EditAnywhere, Category = "JCM")
	USceneComponent* DefaultSceneRootComponent;

	// Custom Static Mesh Component Offset Layer
	UPROPERTY(EditAnywhere, Category = "JCM")
	USceneComponent* CustomActorSceneComponent;

	// Custome Static Mesh Spawn Timer
	FTimerHandle SpawnTimerHandle;

	int32 SpawnCount = 0;

	UPROPERTY()
	int32 SpawnedCustomMeshAmount;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	bool bEnablePhysics;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Creating Mesh
	virtual void CreateSingleCustomMeshComponent(const float BarHeight, const float UnitMeshHeight, int32 SpawnAmount);
	// Creating Mesh Only One
	virtual void CreateSingleCustomMeshComponent(const float UnitMeshHeight);

	virtual void AdjustTextMeshComponentOffsetRoutine(const float InHeight);
	
	// Adjust Value TextRednerComponent Z Offset
	virtual void AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const float& BarHeight);
	virtual void AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const int32& amount);
	virtual void AdjustTextMeshOffset(UTextRenderComponent* TargetTextRednerComponent);

	// ChartGenerator Delegate Binding Functions
	virtual void OnChartGeneratingDoneBindingRoutine();

	// ChartGenerator Searching Delegate Binding Function
	virtual void OnChartSearchingBindingRoutine(int32 InIndex);

public:	
	// chart searching done delegate binding function
	void BindToChartSearching(FChartSearchingDelegate& Delegate);

	// chart generating done delegate binding function
	void BindToChartGeneratingEnd(FChartGeneratingDoneDelegate& Delegate);

	// On : Spawning custom mesh, Off: Spawning default bar mesh
	UPROPERTY(EditAnywhere, Category = "JCM")
	bool bEnableSpawnCustomMesh;

	UPROPERTY(VisibleAnywhere, Category = "JCM")
	UProceduralMeshComponent* ProcMeshComponent;

	// Specify static mesh to generate, Don't forget to check off isProceduralMeshUsing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JCM")
	UStaticMeshComponent* CustomStaticMeshTemplateComponent;

	// Bar procedural mesh material
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!EnableSpawnCustomMesh"), Category = "JCM")
	UMaterialInstance* MeshMaterial;

	// Bar generate animation curve
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!EnableSpawnCustomMesh"), Category = "JCM")
	UCurveFloat* AnimationCurve;

	// Visulalizing each JCM label data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JCM")
	UTextRenderComponent* TextRenderComponentValue;

	// Visualizing each JCM value data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JCM")
	UTextRenderComponent* TextRenderComponentLabel;

	// Basic bar mesh width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!EnableSpawnCustomMesh"), Category = "JCM")
	float Width_bar = 100.f;

	// Spawning custom mesh with user-defined division unit size
	UPROPERTY(EditAnywhere, meta = (EditCondition = "EnableSpawnCustomMesh"), Category = "JCM")
	bool SpawnPerUnitValue= false;

	// Division unit size
	UPROPERTY(EditAnywhere, meta = (EditCondition = "SpawnPerUnitValue"), Category = "JCM")
	float UnitValue=10;

	// Custom mesh spawning delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EnableSpawnCustomMesh"), Category = "JCM")
	float CustomMeshSpawnWaitingTime = 0.5f;

	// TextRender(Value) offset from JCM mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	float TextRenderComponentOffset_Value = 30;

public:
	// Sets default values for this actor's properties
	AJCMBarBaseActor();

	void ClearCustomMeshes();
	void ClearSpawnTimerHandle();

	// Set StaticMeshComponent same as TemplateStaticMeshComponent
	void InitializeStaticMeshProperty(UStaticMeshComponent* TargetStaticMeshComponent, 
		const UStaticMeshComponent* TemplateMeshComponent);

	// Get current custom mesh unit size
	UFUNCTION(BlueprintCallable, Category = "JCM")
	FVector GetStaticMeshBoxUnitSize(UStaticMesh* TargetStaticMesh) const;

	// Animation Control Function
	UFUNCTION()
	void PlayBarAnimation();

	// Create procedural mesh
	void CreateProceduralBoxMesh(float BarHeight);

	// Create custom mesh
	void CreateMesh(float BarHeight, int Value);

	// Initialize Lbel Text Mesh
	UFUNCTION()
	void InitializeTextMeshLabel(const FString& LabelName);

	// Initialize Value Text Mesh
	UFUNCTION()
	void InitializeTextMeshValue(const float& FloatValue);

	// Custom mesh spawned amount
	UFUNCTION(BlueprintCallable, Category = "JCM")
	int32 GetCustomMeshSpawnedAmount() const
	{
		return SpawnedCustomMeshAmount;
	}

	void BindTimelineAnimation();

	// Calculated custom mesh unit height
	UFUNCTION(BlueprintCallable, Category = "JCM")
	float GetCustomMeshUnitHeight();

	// TimeLine Animation Binding Function
	UFUNCTION()
	void OnAnimationUpdate(float Value); // 타임라인에 바인딩될 함수는 UFUNCTION() 붙여야 함

	// Mesh Create Preference Routine
	void CreateCustomMeshRoutine(float BarHeight);
	void CreateCustomMeshRoutine(float BarHeight, int32 amount);
	void CreateCustomMeshRoutine();

	void CreateAdditionalCustomMeshComponent(float BarHeight, float restHeight, float UnitMeshHeight);

	void InitializeCustomStaticMeshPhysics(UStaticMeshComponent* TargetStaticMesh,
		UStaticMeshComponent* TemplateComponent);

	// Set Index from Spline Parent Index
	void SetParentSplineIndex(const int32 InIndex);
	
	// Get Text Value from TextRenderComponent
	int32 GetValueFromTextRenderComponent(UTextRenderComponent* TargetTextRenderComponent);

	// Get Generated Custom StaticMeshComponent at SplineIndex
	UStaticMeshComponent* GetCustomStaticMeshComponent(const int32 SplineIndex);

	
};