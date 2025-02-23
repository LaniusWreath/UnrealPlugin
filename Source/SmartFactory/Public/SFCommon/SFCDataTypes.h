// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFCDataTypes.generated.h"

/**
 * 
 */

// Generaly Usable Data Struct
USTRUCT(BlueprintType)
struct FGenericData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    FString Key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    FString Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    TMap<FString, FString> Attributes;

    FGenericData() :
        Key(""), Value(""), Attributes(){ };
};

// Generaly Usable json Data Struct
USTRUCT(BlueprintType)
struct FJSONData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    TMap<FString, FString> StringFields;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    TMap<FString, int32> IntFields;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    TMap<FString, float> FloatFields;
};

// Generaly Usable CSV Data Struct
USTRUCT(BlueprintType)
struct FGenericDataRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    FString Key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    FString Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC|Data")
    FString Attributes;
};

UCLASS()
class SMARTFACTORY_API USFCDataTypes : public UObject
{
	GENERATED_BODY()
	
};
