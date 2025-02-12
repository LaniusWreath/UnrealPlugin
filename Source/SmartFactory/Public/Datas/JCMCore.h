// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JCMCore.generated.h"

class UJCMDataManager;
class UJCMHttpHandler;
class UJCMWidgetManager;

/**
 * 
 */
UCLASS(Blueprintable)
class SMARTFACTORY_API UJCMCore : public UObject
{
	GENERATED_BODY()

public:
	UJCMCore();

	// Get Jcm Core Instance
	UFUNCTION(BlueprintCallable, Category = "JCM")
	static const UJCMCore* GetJCMCore();

	// Initialize JCM Core
	UFUNCTION(BlueprintCallable, Category = "JCM")
	static const UJCMCore* InitializeJCMCore(UObject* Outer, TSubclassOf<UJCMCore> ManagerClass = nullptr);

	// Initialize JCM Managers
	UFUNCTION(BlueprintCallable, Category = "JCM")
	void InitializeManagers();
	
	// Deleting JCM Core 
	UFUNCTION(BlueprintCallable, Category = "JCM")
	static void JCMCoreDestroy();

	// Get JCM HttpManager Reference
	UFUNCTION(BlueprintCallable, Category = "JCM")
	const UJCMHttpHandler* GetHttpRequestManager() const { return RequestManagerInstance; }

	// Get JCM Data Containing Manager Reference
	UFUNCTION(BlueprintCallable, Category = "JCM")
	UJCMDataManager* GetDataManager() const { return DataManagerInstance; }

	UFUNCTION(BlueprintCallable, Category = "JCM")
	const UJCMWidgetManager* GetWidgetManager() const { return WidgetManagerInstance; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JCM")
	TSubclassOf<class UJCMWidgetManager> WidgetManagerType;

protected:
	virtual void initializeWidgetManager();

private:
	static UJCMCore* JCMCoreInstance;
	UJCMDataManager* GetJCMDataManager();
	UJCMHttpHandler* GetJCMRequestManager();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JCM", meta = (AllowPrivateAccess = true))
	UJCMDataManager* DataManagerInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JCM", meta = (AllowPrivateAccess = true))
	UJCMHttpHandler* RequestManagerInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JCM", meta = (AllowPrivateAccess = true))
	UJCMWidgetManager* WidgetManagerInstance;

};
