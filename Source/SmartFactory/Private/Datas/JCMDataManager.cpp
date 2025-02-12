// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMDataManager.h"
#include "Datas/JCMJsonHandler.h"
#include "Datas/JCMDataContainer.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Datas/JCMLog.h"


// 데이터 매니저 객체 직접 생성 및 전달
UJCMDataManager* UJCMDataManager::CreateJCMDataManagerInstance(UObject* Outer)
{
	return NewObject<UJCMDataManager>(Outer, UJCMDataManager::StaticClass());
}

// json FString 읽어 데이터 컨테이너 반환해주는 
UJCMDataContainer* UJCMDataManager::InstancingDataContainerFromJsonString(const FString& JsonBody, const FString& InHeader)
{
	TSharedPtr<FJsonObject> Data = DeserializeJsonStringToJsonObject(JsonBody);
	FDataInstancePair NewChartData = InstancingDataContainer(Data, InHeader);
	return NewChartData.DataInstance;
}

UJCMDataContainer* UJCMDataManager::GetInstancedDataContainerFromJsonString(UObject* Outer, const FString& JsonBody, const FString& InHeader)
{
	TSharedPtr<FJsonObject> Data = DeserializeJsonStringToJsonObject(JsonBody);
	FDataInstancePair NewChartData = InstancingDataContainerToOuter(Outer, Data, InHeader);
	return NewChartData.DataInstance;
}	

// FString으로 Serialize된 Json문자열 객체로 다시 변환
TSharedPtr<FJsonObject> UJCMDataManager::DeserializeJsonStringToJsonObject(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	// JSON 문자열을 FJsonObject로 파싱
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		return JsonObject;
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("JCMDataManager : Failed to deserialize JSON string."));
		return nullptr;
	}
}

// Create Emtpy BarType Data Container Instance
UJCMDataContainer* UJCMDataManager::CreateEmptyDataContainer(EJCMChartTypes ChartType)
{
	switch (ChartType)
	{
	case NONE:
		break;
	case BAR:
		return NewObject<UJCMDataContainerBar>();
		break;
	case LINE:
		return NewObject<UJCMDataContainerLine>();
		break;
	case PIE:
		return NewObject<UJCMDataContainerPie>();
		break;
	case XY:
		return NewObject<UJCMDataContainerXY>();
		break;
	case XYZ:
		return NewObject<UJCMDataContainerXYZ>();
		break;
	case FREE:
		return NewObject<UJCMDataContainer>();
		break;
	default:
		break;
	}
	return nullptr;
}

// JSON -> FString
FString UJCMDataManager::SerializeJSONToString(const TSharedPtr<FJsonObject> JSONObject)
{
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);

	if (FJsonSerializer::Serialize(JSONObject.ToSharedRef(), Writer))
	{
		UE_LOG(JCMlog, Log, TEXT("JCMDataManager : JSONToString serialized"));
	}
	else
	{
		UE_LOG(JCMlog, Log, TEXT("JCMDataManager : SONToString serialize failed"));
	}

	return JsonString;
}

// 데이터 입력 받아 파싱, DataClass 객체 생성 -> Chart
FDataInstancePair UJCMDataManager::InstancingDataContainer(const TSharedPtr<FJsonObject> Data, const FString& InHeader) 
{
	FDataInstancePair DataPair = FDataInstancePair();

	if (!Data.IsValid())
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : Input Data is invalid"));
		return DataPair;
	}

	// 언리얼은 예외처리 비활성화됨. 하나 씩 필드 유효한지 검사
	FString ChartType;
	if (!Data->TryGetStringField(TEXT("chartType"), ChartType))
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'chartType' is missing or invalid"));
		return DataPair;
	}

	FString ChartTitle;
	if (!Data->TryGetStringField(TEXT("chartTitle"), ChartTitle))
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'ChartTitle' is missing or invalid"));
		return DataPair;
	}

	EJCMChartTypes CurChartTypeEnum = JCMDataTypes::JCMMapChartTypes[ChartType];

	switch (CurChartTypeEnum)
	{
	case NONE:
		break;
	case BAR:
	{
		// 데이터 컨테이너 객체 생성
		UJCMDataContainerBar* NewChartBarClass = NewObject<UJCMDataContainerBar>();

		// X축 데이터 추출
		const TSharedPtr<FJsonObject>* XAxisObject; 
		FString XName;
		TArray<FString> XLabels;
		const TArray<TSharedPtr<FJsonValue>>* LabelArray;

		if (!Data->TryGetObjectField(TEXT("xAxis"), XAxisObject))
		{
			UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'xAxis' is missing or invalid"));
			return DataPair;
		}
		else
		{
			// X축 라벨 이름 추출
			if (!XAxisObject->Get()->TryGetStringField(TEXT("key"), XName))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'key (x)' is missing or invalid"));
				return DataPair;
			}

			// x축 데이터 배열 추출
			if (!XAxisObject->Get()->TryGetArrayField(TEXT("label"), LabelArray))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'label' is missing or invalid"));
				return DataPair;
			}
			else
			{
				for (const TSharedPtr<FJsonValue>& Value : *LabelArray)
				{
					XLabels.Add(Value->AsString());
				}
			}
		}

		// Y축 데이터 추출
		const TSharedPtr<FJsonObject>* YAxisObject;
		FString YName;
		TArray<float> YValues;
		const TArray<TSharedPtr<FJsonValue>>* ValueArray;

		if (!Data->TryGetObjectField(TEXT("yAxis"), YAxisObject))
		{
			UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'yAxis' is missing or invalid"));
			return DataPair;
		}
		else
		{
			// Y축 데이터 이름 추출
			if (!YAxisObject->Get()->TryGetStringField(TEXT("key"), YName))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'key (y)' is missing or invalid"));
				return DataPair;
			}
			// Y축 데이터 배열 추출
			if (!YAxisObject->Get()->TryGetArrayField(TEXT("value"), ValueArray))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'value' is missing or invalid"));
				return DataPair;
			}
			else
			{
				for (const TSharedPtr<FJsonValue>& Value : *ValueArray)
				{
					YValues.Add(Value->AsNumber());
				}
			}
		}
		
		// 여기까지 와야 헤더 붙은 유효한 데이터 컨테이너, 클래스 밖에서 검사할 때는 헤더가 붙어있는지, "" 아닌지 검사하면 됨.
		NewChartBarClass->SetChartData(ChartTitle, ChartType, XName, XLabels, YName, YValues);
		DataPair.Header = InHeader;
		DataPair.DataInstance = NewChartBarClass;
		return DataPair;
	}
		
	case LINE:
		break;
	case PIE:
		break;
	case XY:
		break;
	case XYZ:
		break;
	case FREE:
		break;
	default:
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : Instancing Data Class was failed"));
	}
	return DataPair;
}

// 소유권 명시하여 데이터 컨테이너 인스턴싱
FDataInstancePair UJCMDataManager::InstancingDataContainerToOuter(UObject* Outer, const TSharedPtr<FJsonObject> Data, const FString& InHeader)
{
	FDataInstancePair DataPair = FDataInstancePair();

	if (!Data.IsValid())
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : Input Data is invalid"));
		return DataPair;
	}

	// 언리얼은 예외처리 비활성화됨. 하나 씩 필드 유효한지 검사
	FString ChartType;
	if (!Data->TryGetStringField(TEXT("chartType"), ChartType))
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'chartType' is missing or invalid"));
		return DataPair;
	}

	FString ChartTitle;
	if (!Data->TryGetStringField(TEXT("chartTitle"), ChartTitle))
	{
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : 'ChartTitle' is missing or invalid"));
		return DataPair;
	}

	EJCMChartTypes CurChartTypeEnum = JCMDataTypes::JCMMapChartTypes[ChartType];

	switch (CurChartTypeEnum)
	{
	case NONE:
		break;
	case BAR:
	{
		// 데이터 컨테이너 객체 생성
		UJCMDataContainerBar* NewChartBarClass = NewObject<UJCMDataContainerBar>(Outer);

		// X축 데이터 추출
		const TSharedPtr<FJsonObject>* XAxisObject;
		FString XName;
		TArray<FString> XLabels;
		const TArray<TSharedPtr<FJsonValue>>* LabelArray;

		if (!Data->TryGetObjectField(TEXT("xAxis"), XAxisObject))
		{
			UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'xAxis' is missing or invalid"));
			return DataPair;
		}
		else
		{
			// X축 라벨 이름 추출
			if (!XAxisObject->Get()->TryGetStringField(TEXT("key"), XName))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'key (x)' is missing or invalid"));
				return DataPair;
			}

			// x축 데이터 배열 추출
			if (!XAxisObject->Get()->TryGetArrayField(TEXT("label"), LabelArray))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'label' is missing or invalid"));
				return DataPair;
			}
			else
			{
				for (const TSharedPtr<FJsonValue>& Value : *LabelArray)
				{
					XLabels.Add(Value->AsString());
				}
			}
		}

		// Y축 데이터 추출
		const TSharedPtr<FJsonObject>* YAxisObject;
		FString YName;
		TArray<float> YValues;
		const TArray<TSharedPtr<FJsonValue>>* ValueArray;

		if (!Data->TryGetObjectField(TEXT("yAxis"), YAxisObject))
		{
			UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'yAxis' is missing or invalid"));
			return DataPair;
		}
		else
		{
			// Y축 데이터 이름 추출
			if (!YAxisObject->Get()->TryGetStringField(TEXT("key"), YName))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'key (y)' is missing or invalid"));
				return DataPair;
			}
			// Y축 데이터 배열 추출
			if (!YAxisObject->Get()->TryGetArrayField(TEXT("value"), ValueArray))
			{
				UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : InstancingDataContainer : 'value' is missing or invalid"));
				return DataPair;
			}
			else
			{
				for (const TSharedPtr<FJsonValue>& Value : *ValueArray)
				{
					YValues.Add(Value->AsNumber());
				}
			}
		}

		NewChartBarClass->SetChartData(ChartTitle, ChartType, XName, XLabels, YName, YValues);
		DataPair.Header = InHeader;
		DataPair.DataInstance = NewChartBarClass;
		return DataPair;
	}

	case LINE:
		break;
	case PIE:
		break;
	case XY:
		break;
	case XYZ:
		break;
	case FREE:
		break;
	default:
		UE_LOG(JCMlog, Warning, TEXT("JCMDataManager : Instancing Data Class was failed"));
	}
	return DataPair;
}

