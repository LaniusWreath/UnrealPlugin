// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCDataManageUtilities.h"
#include "SFCommon/SFCLog.h"
#include "SFCommon/SFCDataContainer.h"
#include "JsonUtilities.h"

//--------------------------------- Data Management --------------------------------------------

// 구조체 입력 받아 데이터 컨테이너 인스턴싱
USFCDataContainer* USFCDataManageUtilities::InstancingDataContainer(UObject* Outer, const FGenericData& InData)
{
	USFCDataContainer* NewContainer = NewObject<USFCDataContainer>(Outer);
	if (NewContainer)
	{
		NewContainer->SetData(InData);
	}
	return NewContainer;
}

// 빈 데이터 컨테이너 인스턴싱
USFCDataContainer* USFCDataManageUtilities::CreateEmptyDataContainer(UObject* Outer)
{
	USFCDataContainer* NewContainer = NewObject<USFCDataContainer>(Outer);

	return NewContainer;
}

USFCDataContainer* USFCDataManageUtilities::UpdateDataContainer(USFCDataContainer* InContainer, const FGenericData& InData)
{
	if (!InContainer)
	{
		UE_LOG(SFClog, Warning, TEXT("UpdateDataContainer: InDataContainer is invalid"));
		return nullptr;
	}
	InContainer->SetData(InData);
	return InContainer;
}

// FString을 FGenericData로, FString 내부 json 형태의 key는 정확히 맞아야 함
bool USFCDataManageUtilities::StringToSFCDataStruct(const FString& InString, FGenericData& InDataStruct)
{
    if (InString.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertJsonToGenericData: Input JSON string is empty."));
        return false;
    }

    // Convert FString to FGenericData using JsonObjectStringToUStruct
    if (FJsonObjectConverter::JsonObjectStringToUStruct<FGenericData>(InString, &InDataStruct))
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully converted JSON to FGenericData."));
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to convert JSON to FGenericData."));
        return false;
    }
}

// JsonString을 Map으로
TMap<FString, FString> USFCDataManageUtilities::ParseJsonStringToMap(const FString& JsonString)
{
	TMap<FString, FString> ParsedMap;
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		for (auto& Elem : JsonObject->Values)
		{
			// FJsonValue의 타입에 따라 처리
			if (Elem.Value->Type == EJson::String)
			{
				ParsedMap.Add(Elem.Key, Elem.Value->AsString());
			}
			else if (Elem.Value->Type == EJson::Number)
			{
				ParsedMap.Add(Elem.Key, FString::SanitizeFloat(Elem.Value->AsNumber()));
			}
			else if (Elem.Value->Type == EJson::Object)
			{
				// 중첩된 JSON 객체는 FString으로 변환
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ParsedMap.Add(Elem.Key));
				FJsonSerializer::Serialize(Elem.Value->AsObject().ToSharedRef(), Writer);
			}
			else if (Elem.Value->Type == EJson::Array)
			{
				// 배열은 FString으로 변환
				TArray<TSharedPtr<FJsonValue>> ArrayValues = Elem.Value->AsArray();
				FString ArrayAsString;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ArrayAsString);
				FJsonSerializer::Serialize(ArrayValues, Writer);
				ParsedMap.Add(Elem.Key, ArrayAsString);
			}
			else
			{
				ParsedMap.Add(Elem.Key, TEXT("")); // 기타 경우 빈 문자열
			}
		}
	}
	else
	{
		UE_LOG(SFClog, Warning, TEXT("Failed Json Parsing"));
	}

	return ParsedMap;
}


TArray<FString> USFCDataManageUtilities::ParseStringToStringArray(const FString& ArrayString)
{
	TArray<FString> StringArray;

	// 1. `[` 및 `]` 제거
	FString CleanString = ArrayString;
	CleanString.RemoveFromStart("[");
	CleanString.RemoveFromEnd("]");

	// 2. 쉼표를 기준으로 문자열을 분리
	CleanString.ParseIntoArray(StringArray, TEXT(","), true);

	// 3. 각 요소의 앞뒤 공백 제거
	for (FString& Str : StringArray)
	{
		Str = Str.TrimStartAndEnd();
	}

	return StringArray;
}

TArray<float> USFCDataManageUtilities::ParseStringToFloatArray(const FString& ArrayString)
{
	TArray<float> FloatArray;

	// 1. `[` 및 `]` 제거
	FString CleanString = ArrayString;
	CleanString.RemoveFromStart("[");
	CleanString.RemoveFromEnd("]");

	// 2. 쉼표를 기준으로 문자열을 분리
	TArray<FString> StringArray;
	CleanString.ParseIntoArray(StringArray, TEXT(","), true);

	// 3. 각 문자열 요소를 float로 변환하여 배열에 추가
	for (const FString& Str : StringArray)
	{
		FloatArray.Add(FCString::Atof(*Str));
	}

	return FloatArray;
}

FString USFCDataManageUtilities::ExtractDataFieldFromJsonString(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// Check if the "data" field exists
		TSharedPtr<FJsonObject> DataObject = JsonObject->GetObjectField(TEXT("data"));

		if (DataObject.IsValid())
		{
			// Convert the "data" object back to a JSON string
			FString DataString;
			TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&DataString);

			if (FJsonSerializer::Serialize(DataObject.ToSharedRef(), JsonWriter))
			{
				return DataString;
			}
		}
	}

	return FString(); // Return an empty string if extraction fails
}

bool USFCDataManageUtilities::SaveStringToFile(FString FileName, FString TextToSave)
{
	FString FullPath = FPaths::ProjectDir() + TEXT("Saved/") + FileName;

	return FFileHelper::SaveStringToFile(TextToSave, *FullPath);
}

// ---------------------------------------- Debugging ---------------------------------------------

// FString으로 구성된 Map 멤버들 출력 디버깅용 함수
void USFCDataManageUtilities::PrintStringMapMembers(const TMap<FString, FString>& InMap, const float Duration,
	const FColor TextColor)
{
	for (const TPair<FString, FString>& Pair : InMap)
	{
		UE_LOG(SFClog, Log, TEXT("Key : %s, Value : %s"), *Pair.Key, *Pair.Value);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, Duration, TextColor, FString::Printf(TEXT("Key : %s, Value : %s"), *Pair.Key,
				*Pair.Value));
		}
	}
}

