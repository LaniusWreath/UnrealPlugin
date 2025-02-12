// Fill out your copyright notice in the Description page of Project Settings.

// 블루프린트로 뽑은 뒤, 레벨에 따로 인스턴싱 해야함.
// 인스턴스마다 하나의 json url과 response json 객체를 저장함.

#include "Datas/JCMHttpHandler.h"
#include "Datas/JCMLog.h"


// String Response 함수,request 델리게이트 바인딩 하기위해 명시적으로 재정의
void UJCMHttpHandler::OnResponseReceivedWithString(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	Super::OnResponseReceivedWithString(Request, Response, bWasSuccessful);
}

// Object Response 함수, request 델리게이트 바인딩 하기 위해 명시적으로 재정의
void UJCMHttpHandler::OnResponseReceivedWithPtr(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// 응답 데이터 확인
		ResultResponseString = Response->GetContentAsString();
		//UE_LOG(JCMlog, Log, TEXT("Response: %s"), *ResultResponseString);

		TSharedPtr<FJsonObject> JsonData;

		// JSON 파싱
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResultResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonData) && JsonData.IsValid())
		{
			// 파싱 실행 함수 호출 및 델리게이트 응답 호출
			CallCustomParseFucntion(JsonData);
		}
		else
		{
			UE_LOG(JCMlog, Error, TEXT("%s : Failed to parse JSON."), *this->GetName());
		}
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("%s : HTTP Request failed."), *this->GetName());
	}
}

// 데이터 형식에 맞는 커스텀 파싱 함수 호출. (Object Response 전용)
void UJCMHttpHandler::CallCustomParseFucntion(TSharedPtr<FJsonObject> OriginJsonObject)
{
	ParsedJsonData = ParseRequestBody(OriginJsonObject);
	if (ParsedJsonData)
	{
		//델리게이트 바인딩된 함수 검사 후 execute, 검사 안하면 오류 발생
		if (!OnParsedJsonObjectPtrReady.IsBound())
		{
			UE_LOG(JCMlog, Warning, TEXT("%s : Delegate function not found"), *this->GetName());
		}
		OnParsedJsonObjectPtrReady.ExecuteIfBound(ParsedJsonData);
		OnDynamicRequestingEvent.Broadcast();
	}
}

// HTTPHander 인스턴스 직접 생성하여 소유시키는 함수
UJCMHttpHandler* UJCMHttpHandler::CreateJCMHttpHandlerInstance(UObject* Outer)
{
	return NewObject<UJCMHttpHandler>(Outer, UJCMHttpHandler::StaticClass());
}
