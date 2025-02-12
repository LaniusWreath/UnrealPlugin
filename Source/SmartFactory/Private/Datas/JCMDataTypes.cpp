// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMDataTypes.h"

// TEXT 또는 String을 Enum에 매핑하기 위한 TMap
TMap<FString, EJCMChartTypes> JCMDataTypes::JCMMapChartTypes = 
{
	{TEXT("NONE"), EJCMChartTypes::NONE},
	{TEXT("BAR"), EJCMChartTypes::BAR},
	{TEXT("LINE"), EJCMChartTypes::LINE},
	{TEXT("PIE"), EJCMChartTypes::PIE},
	{TEXT("XY"), EJCMChartTypes::XY},
	{TEXT("XYZ"), EJCMChartTypes::XYZ},
	{TEXT("FREE"), EJCMChartTypes::FREE}
};

JCMDataTypes::JCMDataTypes()
{
	
}

JCMDataTypes::~JCMDataTypes()
{
}