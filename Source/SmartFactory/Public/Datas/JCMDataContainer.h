// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JCMDataTypes.h"
#include "JCMDataContainer.generated.h"

// Mapping Chart Data and Returning Instance Factory Method : Abstract
UCLASS(BlueprintType)
class SMARTFACTORY_API UJCMDataContainer : public UObject
{
	GENERATED_BODY()

private:

protected:
	EJCMChartTypes ChartType = EJCMChartTypes::NONE;

public:
	UFUNCTION()
	virtual const EJCMChartTypes GetChartType() const;

};

UCLASS(BlueprintType)
class UJCMDataContainerBar : public UJCMDataContainer
{
	GENERATED_BODY()

private:
	EJCMChartTypes ChartType = EJCMChartTypes::BAR;
	FJCMChartDataShape ShapeChartData;

public:
	UFUNCTION(BlueprintCallable, Category = "JCM")
	UJCMDataContainerBar* SetChartData(const FJCMChartDataShape& InputData);
	UJCMDataContainerBar* SetChartData(const FString& ChartTitle, const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels,
		const FString& YName, const TArray<float>& Values);

	// Getter Enum Chart Type
	virtual const EJCMChartTypes GetChartType() const override
	{
		return ChartType;
	}

	UFUNCTION(BlueprintCallable, Category = "JCM")
	const FJCMChartDataShape& GetDataStruct() const
	{
		return ShapeChartData;
	}

	UFUNCTION(BlueprintCallable, Category = "JCM")
	const TArray<FText> GetChartDataFTextLabels() const;

	UFUNCTION(BlueprintCallable, Category = "JCM")
	const int32 GetIndexByLabelName(const FString& InLabelName);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	bool DoesLabelExistInArray(const FString& ValueToCheck);
};

UCLASS()
class UJCMDataContainerLine: public UJCMDataContainer
{
	GENERATED_BODY()
	

private:
	EJCMChartTypes ChartType = EJCMChartTypes::LINE;

public:

};

UCLASS()
class UJCMDataContainerPie: public UJCMDataContainer
{
	GENERATED_BODY()

private:
	EJCMChartTypes ChartType = EJCMChartTypes::PIE;

public:

};

//////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class UJCMDataContainerXY : public UJCMDataContainer
{
	GENERATED_BODY()

private:
	EJCMChartTypes ChartType = EJCMChartTypes::XY;
	FJCMChartDataXY XYChartData;

public:

	virtual bool SetChartData(const FString& XName, const TArray<float>& XData, const FString& YName, const TArray<float>& YData);
	UFUNCTION(BlueprintCallable, Category = "JCM")
	virtual bool SetChartData(const FJCMChartDataXY& InputData);
};

UCLASS()
class UJCMDataContainerXYZ : public UJCMDataContainer
{
	GENERATED_BODY()

private:
	EJCMChartTypes ChartType = EJCMChartTypes::XYZ;
	FJCMChartDataXYZ XYZChartData;

public:
	virtual bool SetChartData(const TArray<FString>& LabelNames, const TArray<FVector>& VectorValues);
	UFUNCTION(BlueprintCallable, Category = "JCM")
	virtual bool SetChartData(const FJCMChartDataXYZ& InputData);
};