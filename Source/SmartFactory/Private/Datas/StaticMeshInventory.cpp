// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/StaticMeshInventory.h"
#include "Datas/JCMLog.h"

TArray<UStaticMesh*> UStaticMeshInventory::GetStaticMeshArray()
{
	if (StaticMeshArray.Num() ==0)
	{
		UE_LOG(JCMlog, Warning, TEXT("StaticMeshInventory size is 0"));
	}

	return StaticMeshArray;
}

UStaticMesh* UStaticMeshInventory::GetStaticMesh(const int32 InventoryIndex)
{
	if (!StaticMeshArray.IsValidIndex(InventoryIndex))
	{
		UE_LOG(JCMlog, Warning, TEXT("StaticMeshInventory : Invalid Inventory Index"));
		return nullptr;
	}

	return StaticMeshArray[InventoryIndex];
}

const int32 UStaticMeshInventory::GetAmount(const int32 InventoryIndex)
{
	if (!SafeAmountArray.IsValidIndex(InventoryIndex))
	{
		UE_LOG(JCMlog, Warning, TEXT("StaticMeshInventory : Invalid Inventory Index"));
		// 인덱스 유효하지 않으면 마지막 것 참조
		return 0;
	}

	return SafeAmountArray[InventoryIndex];
}

void UStaticMeshInventory::SetSafeAmountArray(const TArray<int32>& InDataArray)
{
	if (InDataArray.Num() == 0)
	{
		UE_LOG(JCMlog, Warning, TEXT("Inventory Data Array input size is 0"));
	}
	SafeAmountArray = InDataArray;
}
