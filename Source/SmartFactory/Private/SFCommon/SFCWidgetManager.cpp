// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCWidgetManager.h"
#include "Blueprint/UserWidget.h"
#include "SFCommon/SFCLog.h"
#include "SFCommon/SFCDataContainer.h"

// 위젯 매니저의 경우, 블루프린트 전용 함수가 필요하니, 클래스 형태 지정할 수 있게끔 
USFCWidgetManager* USFCWidgetManager::CreateWidgetManagerInstance(UObject* Outer, TSubclassOf<USFCWidgetManager> ManagerClass)
{
	// 클래스 지정 안한 경우
	if (!ManagerClass)
	{
		return NewObject<USFCWidgetManager>(Outer, USFCWidgetManager::StaticClass());
	}

	return NewObject<USFCWidgetManager>(Outer, ManagerClass);
}

UUserWidget* USFCWidgetManager::CreateWidgetFromClass(TSubclassOf<UUserWidget> WidetClass, FName InWidgetName, 
	APlayerController* Owner)
{
	if (WidgetMap.Contains(InWidgetName))
	{
		UE_LOG(SFClog, Warning, TEXT("Widet instanced"));
		return WidgetMap[InWidgetName];
	}

	if (!WidetClass)
	{
		UE_LOG(SFClog, Error, TEXT("Invalid Widget Class"));
		return nullptr;
	}

	// 위젯 기본 소유자
	if (!Owner)
	{
		UE_LOG(SFClog, Error, TEXT("JCMWidgetManager : Owner is invalid"));
		return nullptr;
	}

	// 위젯 생성
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(Owner, WidetClass);
	if (!NewWidget)
	{
		UE_LOG(SFClog, Error, TEXT("JCMWidgetManager : Creating Widget failed"));
		return nullptr;
	}

	// 위젯 맵에 새로 만든 위젯 추가
	WidgetMap.Emplace(InWidgetName, NewWidget);
	return NewWidget;
}

// 위젯 생성 안하고 기존 위젯 레퍼런스 맵에 추가
void USFCWidgetManager::AddWidgetRef(UUserWidget* WidgetRef, FName InWidgetName)
{
	if (!WidgetRef)
	{
		UE_LOG(SFClog, Error, TEXT("Invalid Widget Ref"));
		return;
	}

	if (WidgetMap.Contains(InWidgetName))
	{
		UE_LOG(SFClog, Error, TEXT("Widget Name already exists"));
		return;
	}

	// 위젯 맵에 새로 만든 위젯 추가
	WidgetMap.Emplace(InWidgetName, WidgetRef);
}

void USFCWidgetManager::ShowWidget(FName WidgetName)
{
	if (WidgetMap.Contains(WidgetName))
	{
		UUserWidget* TargetWidget = WidgetMap[WidgetName];
		if (TargetWidget && !TargetWidget->IsInViewport())
		{
			TargetWidget->AddToViewport();
		}
	}
}

void USFCWidgetManager::HideWidget(FName WidgetName)
{
	if (WidgetMap.Contains(WidgetName))
	{
		UUserWidget* TargetWidget = WidgetMap[WidgetName];
		if (TargetWidget && TargetWidget->IsInViewport())
		{
			TargetWidget->RemoveFromViewport();
		}
	}
}

void USFCWidgetManager::ClearWidgetMap()
{
	WidgetMap.Empty();
}

const UUserWidget* USFCWidgetManager::GetWidgetRef(FName InWidgetName) const
{
	if (!WidgetMap.Contains(InWidgetName))
	{
		UE_LOG(SFClog, Error, TEXT("No such widget"));
		return nullptr;
	}

	return WidgetMap[InWidgetName];
}

void USFCWidgetManager::SetPlayerControllerRef(APlayerController* InPlayerController)
{
	if (!InPlayerController)
	{
		UE_LOG(SFClog, Error, TEXT("InPlayerController is invalid"));
		return;
	}
	PlayerControllerRef = InPlayerController;
}

// 데이터 컨테이너 맵에 컨테이너 레퍼런스 추가
void USFCWidgetManager::AddDataContainer(USFCDataContainer* InContainer, const FName InName)
{
	if (!InContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Data Container for %s"), *InName.ToString());
		return;
	}

	DataContainerRefs.Add(InName, InContainer);
	UE_LOG(LogTemp, Log, TEXT("Added Data Container: %s"), *InName.ToString());
}

// 데이터 컨테이너 레퍼런스 가져오기
USFCDataContainer* USFCWidgetManager::GetDataContainer(const FName InName) const
{
	if (USFCDataContainer* const* Ref = DataContainerRefs.Find(InName))
	{
		return *Ref;
	}

	UE_LOG(LogTemp, Warning, TEXT("Data Container not found: %s"), *InName.ToString());
	return nullptr;
}

