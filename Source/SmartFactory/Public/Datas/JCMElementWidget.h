// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JCMElementWidget.generated.h"

/**
 * Data3DActor와 다르게, get과 set은 공통 컨테이너 클래스 타입으로, 
 * 실제 소유는 자기 타입에 맞는 데이터 컨테이너를 클래스가 갖도록 했음. 
 * 캐스팅을 매 번 진행하지 않으므로 안전성 타협 가능
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