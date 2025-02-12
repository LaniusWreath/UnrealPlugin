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

    UPROPERTY(BlueprintReadWrite, Category = "SFC")
    FString Key;

    UPROPERTY(BlueprintReadWrite, Category = "SFC")
    FString Value;

    UPROPERTY(BlueprintReadWrite, Category = "SFC")
    TMap<FString, FString> Attributes;

    FGenericData() :
        Key(""), Value(""), Attributes(){ };
};

// Generaly Usable json Data Struct
USTRUCT(BlueprintType)
struct FJSONData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "SFC")
    TMap<FString, FString> StringFields;

    UPROPERTY(BlueprintReadWrite, Category = "SFC")
    TMap<FString, int32> IntFields;

    UPROPERTY(BlueprintReadWrite, Category = "SFC")
    TMap<FString, float> FloatFields;
};

// Generaly Usable CSV Data Struct
USTRUCT(BlueprintType)
struct FGenericDataRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC")
    FString Key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC")
    FString Value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFC")
    FString Attributes;
};

UCLASS()
class SMARTFACTORY_API USFCDataTypes : public UObject
{
	GENERATED_BODY()
	
};
