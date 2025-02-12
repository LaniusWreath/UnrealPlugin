// Fill out your copyright notice in the Description page of Project Settings

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"
#include "JCMDataTypes.h"
#include "JCMDataManager.generated.h"

class UJCMJsonHandler;
class UJCMDataContainer;
class UJCMDataContainerBar;

// Struct With String Header And Data
USTRUCT(BlueprintType)
struct FDataInstancePair
{
	GENERATED_BODY()

	UPROPERTY()
	FString Header;

	UPROPERTY()
	UJCMDataContainer* DataInstance;

	FDataInstancePair() : Header(""), DataInstance(nullptr) {}
};

// Manage Datas Class and Control those Member Function
UCLASS(Blueprintable, BlueprintType)
class SMARTFACTORY_API UJCMDataManager : public UObject
{
	GENERATED_BODY()
	
private:
	// JsonString to JsonObjectPtr
	TSharedPtr<FJsonObject> DeserializeJsonStringToJsonObject(const FString& JsonString);

	// JSON String Serialization Function
	FString SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject);

public:
	// Create JCMDataContainer Instance
	UFUNCTION(BlueprintCallable, Category = "JCM")
	static UJCMDataManager* CreateJCMDataManagerInstance(UObject* Outer);

	// JsonObject Ptr to Data Container <String Header, JCMDataContainer* DataClassInstance>
	FDataInstancePair InstancingDataContainer(const TSharedPtr<FJsonObject> Data, const FString& InHeader);

	// JsonObject Ptr to Data Container <String Header, JCMDataContainer* DataClassInstance>
	FDataInstancePair InstancingDataContainerToOuter(UObject* Outer, const TSharedPtr<FJsonObject> Data, const FString& InHeader);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	UJCMDataContainer* InstancingDataContainerFromJsonString(const FString& JsonBody, const FString& InHeader);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	UJCMDataContainer* GetInstancedDataContainerFromJsonString(UObject* Outer, const FString& JsonBody, const FString& InHeader);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	UJCMDataContainer* CreateEmptyDataContainer(EJCMChartTypes ChartType);
};

