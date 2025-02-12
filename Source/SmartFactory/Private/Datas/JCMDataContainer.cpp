// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMDataContainer.h"
#include "Datas/JCMLog.h"


// 이 클래스의 객체는 DataManager에서 생성되고 관리 됨. 
// 처리된 데이터 구조는 DataTypes의 구조를 따라가며, 포인터가 아닌 실제 값을 멤버 변수로 지정했음.
// 처리된 데이터 원본이 필요하다면 각 자손 클래스의 Getter 함수를 사용할 것.


///////////////////////////////////////// 데이터 Get ////////////////////////////////////////////////
const EJCMChartTypes UJCMDataContainer::GetChartType() const
{
    return ChartType;
}
///////////////////////////////////////// 데이터 Set ///////////////////////////////////////////////

bool UJCMDataContainerXY::SetChartData(const FJCMChartDataXY& InputData)
{
    return false;
}

bool UJCMDataContainerXY::SetChartData(const FString& XName, const TArray<float>& XData, const FString& YName, const TArray<float>& YData)
{
    return false;
}

bool UJCMDataContainerXYZ::SetChartData(const FJCMChartDataXYZ& InputData)
{
    return false;
}

bool UJCMDataContainerXYZ::SetChartData(const TArray<FString>& LabelNames, const TArray<FVector>& VectorValues)
{
    return false;
}

UJCMDataContainerBar* UJCMDataContainerBar::SetChartData(const FJCMChartDataShape& InputData)
{
    ShapeChartData = InputData;
    if (ShapeChartData.ChartType == "")
    {
        UE_LOG(JCMlog, Warning, TEXT("%s : ChartType is Empty"), *this->GetName());
        return nullptr;
    }
    if (ShapeChartData.XName == "" || ShapeChartData.YName == "")
    {
        UE_LOG(JCMlog, Warning, TEXT("%s : Name is Empty"), *this->GetName());
        return nullptr;
    }
    if (ShapeChartData.Labels.Num() == 0 || ShapeChartData.Values.Num() == 0)
    {
        UE_LOG(JCMlog, Warning, TEXT("%s : Data is Empty"), *this->GetName());
        return nullptr;
    }
    
    return this;
}

UJCMDataContainerBar* UJCMDataContainerBar::SetChartData(const FString& ChartTitle, const FString& ChartTypeName, const FString& XName, const TArray<FString>& Labels, const FString& YName, const TArray<float>& Values)
{
    ShapeChartData.ChartTitle = ChartTitle;
    ShapeChartData.ChartType = ChartTypeName;
    ShapeChartData.XName = XName;
    ShapeChartData.Labels = Labels;
    ShapeChartData.YName = YName;
    ShapeChartData.Values = Values;

    if (ShapeChartData.ChartType == "")
    {
        UE_LOG(JCMlog, Warning, TEXT("%s ChartType is empty"), *this->GetName());
        return nullptr;
    }
    if (ShapeChartData.XName == "" || ShapeChartData.YName == "")
    {
        UE_LOG(JCMlog, Warning, TEXT("%s Name is empty"), *this->GetName());
        return nullptr;
    }
    if (ShapeChartData.Labels.Num() == 0 || ShapeChartData.Values.Num() == 0)
    {
        UE_LOG(JCMlog, Warning, TEXT("%s Data is empty"), *this->GetName());
        return nullptr;
    }

    return this;
}

// 데이터 라벨 데이터 FText로 변환하여 리턴
const TArray<FText> UJCMDataContainerBar::GetChartDataFTextLabels() const
{
    TArray<FText> ReturnArray;

    for (const FString& Label : ShapeChartData.Labels)
    {
        ReturnArray.Emplace(FText::FromString(Label));
    }

    return ReturnArray;
}

// 라벨 이름으로 인덱스 찾기
const int32 UJCMDataContainerBar::GetIndexByLabelName(const FString& InLabelName)
{
    int32 Index = ShapeChartData.Labels.IndexOfByKey(InLabelName);
    if (Index == INDEX_NONE)
    {
        UE_LOG(JCMlog, Warning, TEXT("Value '%s' not found in the array"), *InLabelName);
        return 0;
    }

    return Index;
}

bool UJCMDataContainerBar::DoesLabelExistInArray(const FString& ValueToCheck)
{
    return ShapeChartData.Labels.Contains(ValueToCheck);
}
