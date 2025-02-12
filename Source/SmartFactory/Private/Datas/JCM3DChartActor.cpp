	// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCM3DChartActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Datas/JCMCore.h"
#include "Datas/JCMBarBaseActor.h"
#include "Datas/JCMDataManager.h"
#include "Datas/JCMChartGenerator.h"
#include "Datas/JCMDataContainer.h"
#include "Datas/JCMHttpHandler.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"
#include "Datas/JCMLog.h"


/// <summary>
/// BaseActor Functions
/// </summary>
// json을 받아와 DataClass라는 상위 데이터 컨테이너 클래스에 공통으로 저장하고있음.
// 또한 이 공통 데이터는 공통 액터 추상클래스에 속해있음
// 블루프린트에서는 액터가 자식 클래스로 구체화되어있으므로, 데이터 컨테이너 클래스를 호출 할 때에도 액터 형식에 맞게
// 캐스팅해주는 과정이 필요함. 다른 자식 3DActor 클래스에도 구성해줄 것.
// 
// 1205 수정 : 외부로 나가는 get과 set 함수에서는 DataContainer로, 내부에서는 클래스에 맞게 갖고 있게끔 수정
// 1206 수정 : 부모 3DChartActor는 DataContainer 가지고, 자식 3DChartActorBar는 DataContainerBar를 가지게끔 분리
//

AJCM3DChartActor::AJCM3DChartActor()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));
	TextRenderComponent_chartTitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_title"));
	TextRenderComponent_chartTitle->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartTitle->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartTitle->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

// Called when the game starts or when spawned
void AJCM3DChartActor::BeginPlay()
{
	Super::BeginPlay();
}

// JsonObjectPtr를 받는 함수
void AJCM3DChartActor::RequestJsonObject(const FString& URL, const FString& InDataLabel)
{
	SetJCMDataManagerRef();
	InitializeRequestHandler();
	// Request 델리게이트 바인딩 함수 : DataClass 멤버변수 초기화 
	// 멀티캐스트 델리게이트였다면 델리게이트 객체 매 번 청소해야 겠지만, 지금은 싱글캐스트. 객체 매 번 삭제 안해도 됨.

	SetNextDataHeader(InDataLabel);
	RequestHandlerInstance->OnParsedJsonObjectPtrReady.BindUObject(this, &AJCM3DChartActor::CallInstancingDataContainer);
	SetbDataContainerSet(false);
	RequestHandlerInstance->MakeGetRequest(URL, false);
}

// FString을 받는 함수
void AJCM3DChartActor::RequestJsonString(const FString& URL)
{
	SetJCMDataManagerRef();
	InitializeRequestHandler();
	RequestHandlerInstance->OnRequestedJsonStringReady.BindUObject(this, &AJCM3DChartActor::SetbRequestingJsonStringWorkDone);
	bDataContainerSet = false;
	RequestHandlerInstance->MakeGetRequest(URL);
}

// 기본 액터 무결성 체크 함수
bool AJCM3DChartActor::CheckJCMActorIntegrity()
{
	bool bIsValid= true;
	if (!DataManagerRef)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : DataManager is Invalid "), *this->GetName());
		bIsValid = false;
	}

	return bIsValid;
}

void AJCM3DChartActor::SetJCMDataManagerRef()
{	
	if (!DataManagerRef)
	{
		if (UJCMCore::GetJCMCore())
		{
			DataManagerRef = UJCMCore::GetJCMCore()->GetDataManager();
			if (!DataManagerRef)
			{
				UE_LOG(JCMlog, Warning, TEXT("%s: Getting DataManager Reference Failed"), *this->GetName());
			}
		}
		else
		{
			UE_LOG(JCMlog, Warning, TEXT("%s: Couldn't find JCMCore"), *this->GetName());
		}
	}
}

// RequestManager의 인스턴스는 각각 request를 담당하는 액터에서 직접 생성. 
// 각 인스턴스의 델리게이트는 각 액터의 url에 대한 리퀘스트만 감지하여 각 액터 인스턴스의 함수와 1:1 바인딩
// url로 데이터를 리퀘스트 할 때 마다 기존 httpManager인스턴스를 삭제하고 새롭게 초기화.
const UJCMHttpHandler* AJCM3DChartActor::InitializeRequestHandler()
{
	if (!RequestHandlerInstance)
	{
		RequestHandlerInstance = NewObject<UJCMHttpHandler>();
	}
	return RequestHandlerInstance;
}


// FJsonObject 받아 DataManager에게 UDataClasses*로 추출 요청
void AJCM3DChartActor::CallInstancingDataContainer(const TSharedPtr<FJsonObject> JsonData)
{
	if (!RequestHandlerInstance)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: RequestManagerInstance is Invalid"), *this->GetName());
		return;
	}

	FDataInstancePair ResultData = DataManagerRef->InstancingDataContainer(JsonData, NextDataHeader);
	if (ResultData.Header.IsEmpty())
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Received source data is invalid "), *this->GetName());
		return;
	}

	UJCMDataContainer* NewDataContainer = ResultData.DataInstance;
	if (!NewDataContainer)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Data Container instancing failed"), *this->GetName());
	}

	DataContainer = NewDataContainer;
	SetbDataContainerSet(true);
}

// 
void AJCM3DChartActor::SetbRequestingJsonStringWorkDone()
{
	
}

void AJCM3DChartActor::SetbDataContainerSet(const bool InState)
{
	bDataContainerSet = InState;
}

const bool AJCM3DChartActor::GetbDataContainerSet() const
{
	return bDataContainerSet;
}

void AJCM3DChartActor::SetNextDataHeader(const FString& InHeader)
{
	NextDataHeader = InHeader;
}

// Called every frame
void AJCM3DChartActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/// <AJCM3DChartActor>
/// 
/// <AJCM3DChartActorBar>

AJCM3DChartActorBar::AJCM3DChartActorBar()
{
	EnableGenerateMeshAtSplinePoint = false;
	ChartGeneratorComponent = CreateDefaultSubobject<UJCMChartGeneratorBar>(TEXT("BarGeneratorComponent"));
	// 처음에는 ChartGenerator 생성자에서 각각 컴포넌트 붙였으나, 계층 구조만 변경될 뿐 실제로 붙지 않는 문제 발생.
	ChartGeneratorComponent->SetupAttachment(RootSceneComponent);
	// ChartGenerator의 멤버 컴포넌트 따로 부착해주는 함수 작성
	ChartGeneratorComponent->SetAttachComponents(ChartGeneratorComponent);

	TextRenderComponent_chartXaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_xAxis"));
	TextRenderComponent_chartXaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartXaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartXaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

	TextRenderComponent_chartYaxisName = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text_yAxis"));
	TextRenderComponent_chartYaxisName->SetupAttachment(RootSceneComponent);
	TextRenderComponent_chartYaxisName->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponent_chartYaxisName->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
}

const UJCMDataContainerBar* AJCM3DChartActorBar::GetDataContainerRef()
{
	if (!bDataContainerSet)
	{
		UE_LOG(JCMlog, Warning, TEXT("%s :DataContainer not found"), *this->GetName());
		return nullptr;
	}
	return DataContainerBar;
}

void AJCM3DChartActorBar::SetDataContainer(UJCMDataContainer* DataContainerRef)
{
	UJCMDataContainerBar* NewContainer = CastChecked<UJCMDataContainerBar>(DataContainerRef);
	DataContainerBar = NewContainer;
}



// 차트 타이틀, X축, Y축 이름 초기화 함수
void AJCM3DChartActorBar::SetChartDefaultTexts()
{
	if (!DataContainerBar)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : not found DataContainerBar"), *this->GetName());
		return;
	}

	FString ChartTitle = DataContainerBar->GetDataStruct().ChartTitle;
	TextRenderComponent_chartTitle->SetText(FText::FromString(ChartTitle));

	FString ChartXAxisName = DataContainerBar->GetDataStruct().XName;
	TextRenderComponent_chartXaxisName->SetText(FText::FromString(ChartXAxisName));

	FString ChartYAxisName = DataContainerBar->GetDataStruct().YName;
	TextRenderComponent_chartYaxisName->SetText(FText::FromString(ChartYAxisName));
}

void AJCM3DChartActorBar::GenerateChartRoutine()
{
	if (!ChartGeneratorComponent)
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : GenerateChartRoutine : DataContainer is invalid"), *this->GetName());
		return;
	}

	// 차트 타이틀 초기화
	SetChartDefaultTexts();

	// 생성할 바 소스 액터 BarGenerator에 전달
	if (ChartGeneratorComponent && BarBaseActorBPClass)
	{
		ChartGeneratorComponent->SetBarSourceActor(BarBaseActorBPClass);
	}
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : BarBaseActorBPClass or BarGeneratorComponent is invalid"), *this->GetName());
		return;
	}

	if (!DataContainerBar)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : DataContainer is invalid"), *this->GetName());
		return;
	}

	// GenerateBarChart() : 데이터 입력 받아 차트 생성 루틴 함수 호출 / GetShapeChartData() : Bar(모양)차트 데이터 Get
	ChartGeneratorComponent->GenerateBarChart(DataContainerBar->GetDataStruct(), EnableGenerateMeshAtSplinePoint);
}

void AJCM3DChartActorBar::UpdateChartRoutine()
{
	if (!ChartGeneratorComponent)
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : GenerateChartRoutine : DataContainer is invalid"), *this->GetName());
		return;
	}
}


// Bar 액터 무결성 체크 함수
bool AJCM3DChartActorBar::CheckJCMActorIntegrity()
{
	bool bIsValid = Super::CheckJCMActorIntegrity();

	if (!DataContainerBar)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : DataContainer is Invalid"), *this->GetName());
		bIsValid = false;
	}

	if (!ChartGeneratorComponent)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : BarGeneratorComponent Invalid"), *this->GetName());
		bIsValid = false;
	}
	if (!BarBaseActorBPClass)
	{
		UE_LOG(JCMlog, Warning, TEXT("Integrity Check : %s : BarBaseActorBPClass Invalid"), *this->GetName());
		bIsValid = false;
	}

	return bIsValid;
}

// DataContainer-> DataContainerBar로 캐스팅 후 배치
void AJCM3DChartActorBar::CallInstancingDataContainer(const TSharedPtr<FJsonObject> JsonData)
{
	if (!RequestHandlerInstance)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: RequestManagerInstance is Invalid"), *this->GetName());
		return;
	}

	FDataInstancePair ResultData = DataManagerRef->InstancingDataContainer(JsonData, NextDataHeader);
	if (ResultData.Header.IsEmpty())
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Received source data is invalid "), *this->GetName());
		return;
	}

	UJCMDataContainerBar* NewDataContainer = Cast<UJCMDataContainerBar>(ResultData.DataInstance);
	if (!NewDataContainer)
	{
		UE_LOG(JCMlog, Error, TEXT("%s: Data Container instanci failed"), *this->GetName());
		return;
	}

	DataContainerBar = NewDataContainer;
	SetbDataContainerSet(true);
}

