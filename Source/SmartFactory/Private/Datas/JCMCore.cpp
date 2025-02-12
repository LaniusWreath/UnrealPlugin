// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/JCMCore.h"
#include "Datas/JCMDataManager.h"
#include "Datas/JCMHttpHandler.h"
#include "Datas/JCMWidgetManager.h"
#include "Datas/JCMLog.h"


UJCMCore* UJCMCore::JCMCoreInstance = nullptr;

UJCMCore::UJCMCore()
{
	WidgetManagerType = nullptr;
	WidgetManagerInstance = nullptr;
}

// JCMCore 인스턴스 생성 
const UJCMCore* UJCMCore::InitializeJCMCore(UObject* Outer, TSubclassOf<UJCMCore> ManagerClass)
{
	// 지정 안할경우 기본
	if (!ManagerClass)
	{
		ManagerClass = UJCMCore::StaticClass();
	}
	// 지정한 경우 블루프린트 클래스
	JCMCoreInstance = NewObject<UJCMCore>(Outer, ManagerClass);

	if (!JCMCoreInstance)
	{
		UE_LOG(JCMlog, Error, TEXT("Failed instancing JCMCore"));
		return nullptr;
	}

	return JCMCoreInstance;
}

void UJCMCore::InitializeManagers()
{
	JCMCoreInstance->DataManagerInstance = NewObject<UJCMDataManager>();
	JCMCoreInstance->RequestManagerInstance = NewObject<UJCMHttpHandler>();

	//JCMCoreInstance->WidgetManagerInstance = NewObject<UJCMWidgetManager>();
	initializeWidgetManager();
}

// 위젯 매니저 인스턴스 생성
void UJCMCore::initializeWidgetManager()
{
	// 블루프린트상에서 다른 위젯 매니저를 선택했을 경우
	if (WidgetManagerType)
	{
		WidgetManagerInstance = NewObject<UJCMWidgetManager>(this, WidgetManagerType);
	}
	//기본 타입
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AClass is not set! Default UA instance will be created."));
		WidgetManagerInstance = NewObject<UJCMWidgetManager>(this); // 기본 위젯 생성
	}
}


// Get JCMCore Instance
const UJCMCore* UJCMCore::GetJCMCore()
{
	if (JCMCoreInstance)
	{
		return JCMCoreInstance;
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("No JCM Core Instance. Please Initialize Core First"));
		return nullptr;
	}
}

// RequestHandler를 싱글톤으로 매니저처럼 사용 : 소스용
UJCMHttpHandler* UJCMCore::GetJCMRequestManager()
{
	if (!RequestManagerInstance)
	{
		UE_LOG(JCMlog, Warning, TEXT("No JCM Core Instance. Please Initialize Core First"));
	}
	return RequestManagerInstance;
}

// Get JCMDataManager static Instance : 소스용
UJCMDataManager* UJCMCore::GetJCMDataManager()
{
	// DataManager 인스턴스 생성 및 초기화
	if (!DataManagerInstance)
	{
		UE_LOG(JCMlog, Warning, TEXT("No JCM Core Instance. Please Initialize Core First"));
	}
	return DataManagerInstance;
}

// 코어 인스턴스 삭제 및 GC 방지 해제
void UJCMCore::JCMCoreDestroy()
{
	if (JCMCoreInstance)
	{
		JCMCoreInstance->RemoveFromRoot();  // GC 방지 해제
		JCMCoreInstance = nullptr;
	}
}
