// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JCMDataTypes.generated.h"

// Commonly use delegate
DECLARE_DELEGATE(FChartGeneratingDoneDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FChartSearchingDelegate, int32);

// Struct for Shape Chart Data
USTRUCT(BlueprintType)
struct FJCMChartDataShape
{
	GENERATED_BODY()

	FJCMChartDataShape()
	{
		ChartTitle = TEXT("Title");
		ChartType = TEXT("Type");
		XName = TEXT("XName");
		Labels = { TEXT("Label0") };
		YName = TEXT("YName");
		Values = { -1 };
	}

	FJCMChartDataShape(const FString& InTitle, const FString& InType, const FString& InXName, const TArray<FString>& InLabels,
		const FString& InYName, const TArray<float>& InValues)
		: ChartTitle(InTitle), ChartType(InType), XName(InXName), Labels(InLabels), YName(InYName), Values(InValues)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	FString ChartTitle;

	// Chart Types : "bar", "line", "pie" .. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	FString ChartType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	FString XName;

	// Label for X Axis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	TArray<FString> Labels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	FString YName;
	
	// Value for Y Axis or Quotiont for PieChart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	TArray<float> Values;
};

USTRUCT(BlueprintType)
struct FJCMChartDataXY
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	FString ChartTitle;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	FString XName;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	TArray<float> XData;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	FString YName;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	TArray<float> YData;
};

USTRUCT(BlueprintType)
struct FJCMChartPoint
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	FString Name;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	FVector Coordinate;
};

USTRUCT(BlueprintType)
struct FJCMChartDataXYZ
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	FString ChartTitle;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "JCM")
	TArray<FJCMChartPoint> Points;
};

// ¿­°ÅÇü
UENUM(BlueprintType)
enum EJCMChartTypes : uint8
{
	NONE = 0 UMETA(DisplayName = "NONE"),
	BAR = 1 UMETA(DisplayName = "BAR"),
	LINE = 2 UMETA(DisplayName = "LINE"),
	PIE = 3 UMETA(DisplayName = "PIE"),
	XY = 4 UMETA(DisplayName = "XY"),
	XYZ = 5 UMETA(DisplayName = "XYZ"),
	FREE = 6 UMETA(DisplayName = "FREE"),
};

class SMARTFACTORY_API JCMDataTypes
{
public:
	JCMDataTypes();

	UPROPERTY(VisibleAnywhere, Category = "JCM")
	static TMap<FString, EJCMChartTypes> JCMMapChartTypes ;

	~JCMDataTypes();
};


