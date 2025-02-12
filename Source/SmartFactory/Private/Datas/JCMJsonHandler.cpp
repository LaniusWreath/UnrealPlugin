// Fill out your copyright notice in the Description page of Project Settings.

#include "Datas/JCMJsonHandler.h"
#include "Datas/JCMLog.h"


TSharedPtr<FJsonObject> UJCMJsonHandler::GetJsonObjectData(const FString& FilePath)
{
	FString JsonData;
    TSharedPtr<FJsonObject> ParsedData;
    if (FFileHelper::LoadFileToString(JsonData, *FilePath))
    {
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonData);

        // Json 읽기
        if (FJsonSerializer::Deserialize(Reader, ParsedData))
        {
           // UE_LOG(JCMlog, Log, TEXT("JSONHandler : JSON Data Parsed Successfully"));
            return ParsedData;
        }
        else
        {
            UE_LOG(JCMlog, Error, TEXT("JCMDataManager : JSONHander : Failed to Parse JSON Data"));
            return nullptr;
        }
    }
    else
    {
        UE_LOG(JCMlog, Error, TEXT("JCMDataManager : JSONHandler : Failed to Read JSON Data"));
        return nullptr;
    }
}