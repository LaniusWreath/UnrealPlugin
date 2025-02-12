// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JCMElementWidget.generated.h"

/**
 * Data3DActor�� �ٸ���, get�� set�� ���� �����̳� Ŭ���� Ÿ������, 
 * ���� ������ �ڱ� Ÿ�Կ� �´� ������ �����̳ʸ� Ŭ������ ������ ����. 
 * ĳ������ �� �� �������� �����Ƿ� ������ Ÿ�� ����
 */

UCLASS(BlueprintType)
class SMARTFACTORY_API UJCMElementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "JCM")
	void WidgetRedraw();

protected:
	

private:
	
};

class UJCMDataContainerBar;

UCLASS()
class SMARTFACTORY_API UJCMElementBarWidget : public UJCMElementWidget
{
	GENERATED_BODY()

public:
	UJCMDataContainerBar* CastDataContainer(UJCMDataContainer* InDataContainer);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	void SetDataContainerRef(UJCMDataContainer* UJCMDataContainer);

	UFUNCTION(BlueprintCallable, Category = "JCM")
	const UJCMDataContainer* GetDataContainer() const;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JCM", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UJCMDataContainerBar> DataContainerBarRef;

};