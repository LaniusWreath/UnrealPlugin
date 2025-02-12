// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMElementWidget.h"
#include "Datas/JCMDataContainer.h"
#include "Datas/JCMLog.h"

UJCMDataContainerBar* UJCMElementBarWidget::CastDataContainer(UJCMDataContainer* InDataContainer)
{
	UJCMDataContainerBar* DataRef = CastChecked<UJCMDataContainerBar>(InDataContainer);
	return DataRef;
}

void UJCMElementBarWidget::SetDataContainerRef(UJCMDataContainer* UJCMDataContainer)
{
	DataContainerBarRef = CastDataContainer(UJCMDataContainer);
}

const UJCMDataContainer* UJCMElementBarWidget::GetDataContainer() const
{
	return DataContainerBarRef;
}

void UJCMElementWidget::WidgetRedraw()
{
}
