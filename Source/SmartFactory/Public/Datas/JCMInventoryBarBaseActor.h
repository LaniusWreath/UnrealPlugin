// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Datas/JCMBarBaseActor.h"
#include "JCMInventoryBarBaseActor.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EMaterialState : uint8 
{
	DEFAULT UMETA(DisplayName = "Default"), 
	ALARM UMETA(DisplayName = "Alarm"),
	SEARCH UMETA(DisplayName = "Search"),
	NONE      UMETA(Hidden) // 숨겨진 값 (사용하지 않음)
};

UCLASS()
class SMARTFACTORY_API AJCMInventoryBarBaseActor : public AJCMBarBaseActor
{
	GENERATED_BODY()
	
public:
	AJCMInventoryBarBaseActor();

	// Set StaticMesh in StaticMeshComponent with Mesh Inventory
	void InitializeStaticMeshPropertyFromInventory(UStaticMeshComponent* TargetStaticMeshComponent,
		const int32 InInventoryIndex);

	// Get StaticMesh from Inventory
	UStaticMesh* GetStaticMeshFromInventory(const int32 InInventoryIndex);

	// Creating Mesh
	void CreateSingleCustomMeshComponent(const float BarHeight, const float UnitMeshHeight, int32 SpawnAmount) override;
	// Creating Mesh Only One
	void CreateSingleCustomMeshComponent(const float UnitMeshHeight) override;

	void InitializeItemStaticMeshPrepertyFromTemplate(UStaticMeshComponent* TargetStaticMeshComponent, const int32 InventoryIndex);
	
	// Scaling Target Mesh Bounds same as TemplateMesh Bounds
	void ScaleStaticMeshToTemplateBounds(UStaticMeshComponent* NewMesh, UStaticMeshComponent* TemplateMesh);
	
	// Binding Rotation Timer Function
	void InitializeItemMeshRotation(UStaticMeshComponent* TargetStaticMeshComponent, const float InRotationSpeed);

	void AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const float& BarHeight) override;
	void AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const int32& amount) override;
	
	// Call TextRenderComponent offset adjusting functions
	void AdjustTextMeshComponentOffsetRoutine(const float InHeight) override;

	// Initilize TextRenderComponent Max Value
	void InitializeTextRenderComponentMaxValue(UTextRenderComponent* TargetTextRenderComponent, const int32 InInventoryIndex);

	// ChartGenerator Delegate Binding Functions
	void OnChartGeneratingDoneBindingRoutine() override;
	void AlarmSafeAmount();
	void OnChartSearchingBindingRoutine(int32 InIndex) override;
	void ChangeSearchedComponentMaterial(const int32 InIndex);

	// Change CoverMeshMaterial
	UStaticMeshComponent* ChangeStaticMeshComponentMaterial(UStaticMeshComponent* TargetStaticMeshComponent , UMaterialInterface* InMaterial);

	// Update TextRenderComponent Text
	void UpdateData(const int CurrentAmount, const int SafeAmount, const FString& ItemLabel);

	void OnSearched();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEnableItemRotation"), Category = "JCM")
	float RotationSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseStaticMeshInventory"), Category = "JCM")
	bool bEnableItemRotation = false;

private:

	FTimerHandle RotationTimerHandle;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableSpawnCustomMesh", AllowPrivateAccess = "true"), Category = "JCM")
	bool bUseStaticMeshInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseStaticMeshInventory", AllowPrivateAccess = "true"), Category = "JCM")
	TObjectPtr<UStaticMeshComponent> TemplateItemMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	TObjectPtr<class UTextRenderComponent> TextRenderComponentSafeValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	TObjectPtr<UMaterialInterface> AlarmMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	TObjectPtr<UMaterialInterface> DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	TObjectPtr<UMaterialInstance> SearchedMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	EMaterialState CurrentMaterialState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "JCM")
	EMaterialState PreMaterialState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", EditCondition = "bUseStaticMeshInventory"), 
		Category = "JCM")
	TObjectPtr<class UStaticMeshInventory> InventoryDataAsset;
};
