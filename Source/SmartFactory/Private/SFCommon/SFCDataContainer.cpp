// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCDataContainer.h"

USFCDataContainer::USFCDataContainer()
{
	Data = FGenericData();
}

USFCDataContainer* USFCDataContainer::SetData(const FGenericData& InputData)
{
	Data = InputData;
	return this;
}
