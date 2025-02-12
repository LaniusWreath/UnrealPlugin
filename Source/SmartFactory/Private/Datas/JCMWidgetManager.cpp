// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMWidgetManager.h"
#include "Blueprint/UserWidget.h"
#include "Datas/JCMLog.h"

// 위젯 클래스 이름으로 캐싱
UUserWidget* UJCMWidgetManager::CreateWidgetFromClass(TSubclassOf<UUserWidget> WidetClass, FName InWidgetName, 
	APlayerController* Owner)
{
	if (WidgetMap.Contains(InWidgetName))
	{
		UE_LOG(JCMlog, Warning, TEXT("Widet instanced"));
		return WidgetMap[InWidgetName];
	}

	if (!WidetClass)
	{
		UE_LOG(JCMlog, Error, TEXT("Invalid Widget Class"));
		return nullptr;
	}

	// 위젯 기본 소유자
	if (!Owner)
	{
		UE_LOG(JCMlog, Error, TEXT("JCMWidgetManager : Owner is invalid"));
		return nullptr;
	}

	// 위젯 생성
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(Owner, WidetClass);
	if (!NewWidget)
	{
		UE_LOG(JCMlog, Error, TEXT("JCMWidgetManager : Creating Widget failed"));
		return nullptr;
	}

	// 위젯 맵에 새로 만든 위젯 추가
	WidgetMap.Emplace(InWidgetName, NewWidget);
	return NewWidget;
}

// 위젯 뷰포트에 추가
void UJCMWidgetManager::ShowWidget(FName WidgetName)
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

// 위젯 뷰포트에서 삭제
void UJCMWidgetManager::HideWidget(FName WidgetName)
{
	if (WidgetMap.Contains(WidgetName))
	{
		UUserWidget* TargetWidget = WidgetMap[WidgetName];
		if (TargetWidget && TargetWidget->IsInViewport())
		{
			TargetWidget->RemoveFromParent();
		}
	}
}

void UJCMWidgetManager::ClearWidgetMap()
{
	WidgetMap.Empty();
}

const UUserWidget* UJCMWidgetManager::GetWidgetFromClass(FName InWidgetName) const
{
	if (!WidgetMap.Contains(InWidgetName))
	{
		UE_LOG(JCMlog, Error, TEXT("No such widget"));
		return nullptr;
	}

	return WidgetMap[InWidgetName];
}
