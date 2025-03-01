// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFCDataTypes.h"
#include "SFCDataContainer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SMARTFACTORY_API USFCDataContainer : public UObject
{
	GENERATED_BODY()
	
private: 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SFC", 
		meta = (AllowPrivateAccess = "true"))
	FGenericData Data;

public:
	USFCDataContainer();

	UFUNCTION(BlueprintCallable, Category = "SFC|Container")
	USFCDataContainer* SetData(const FGenericData& InputData);

	UFUNCTION(BlueprintCallable, Category = "SFC|Container")
	const FGenericData& GetData() const
	{
		return Data;
	}
};
