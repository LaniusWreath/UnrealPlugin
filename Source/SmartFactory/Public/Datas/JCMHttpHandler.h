// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFCommon/SFCHttpManager.h"
#include "JCMHttpHandler.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class SMARTFACTORY_API UJCMHttpHandler : public USFCHttpManager
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "JCM")
	static UJCMHttpHandler* CreateJCMHttpHandlerInstance(UObject* Outer);

private:

	// HTTP Processing
	void OnResponseReceivedWithString(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;

	void OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;

	void CallCustomParseFucntion(TSharedPtr<FJsonObject> OriginJsonObject) override;

};