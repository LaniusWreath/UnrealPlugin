// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFCDataTypes.h"
#include "SFCDataManageUtilities.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class SMARTFACTORY_API USFCDataManageUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//--------------------------------- DataManagement --------------------------------------------

	// Instancing GenericDataContainer from Struct input
	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Container")
	static USFCDataContainer* InstancingDataContainer(UObject* Outer, const FGenericData& InData);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Container")
	static USFCDataContainer* CreateEmptyDataContainer(UObject* Outer);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Container")
	static USFCDataContainer* UpdateDataContainer(USFCDataContainer* InContainer, const FGenericData& InData);

	// ------------------------------ UtilityFunction ------------------------------------

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Utility")
	static FGenericData StringToSFCDataStruct(const FString& InString);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Utility")
	static TMap<FString, FString> ParseJsonStringToMap(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Utility")
	static TArray<FString> ParseStringToStringArray(const FString& ArrayString);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Utility")
	static TArray<float> ParseStringToFloatArray(const FString& ArrayString);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Utility")
	static FString ExtractDataFieldFromJsonString(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagerment|Utility")
	static bool SaveStringToFile(FString FileName, FString TextToSave);
	
	// ------------------------------ Debuging -------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "SFC|DataManagement|Debugging")
	static void PrintStringMapMembers(const TMap<FString, FString>& InMap, const float Duration = 5, 
		const FColor TextColor = FColor::Green);
};
