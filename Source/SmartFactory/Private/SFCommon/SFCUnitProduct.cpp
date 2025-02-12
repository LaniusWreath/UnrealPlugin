// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/SFCUnitProduct.h"
#include "SFCommon/SFCLog.h"

// Sets default values
ASFCUnitProduct::ASFCUnitProduct()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASFCUnitProduct::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASFCUnitProduct::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASFCUnitProduct::RotateMesh(float InDeltaTime)
{
	if(!CenterMesh)
	{
		UE_LOG(SFClog, Error, TEXT("%s : Make sure target mesh"), *this->GetName());
		return;
	}

	// 현재 회전값 가져오기
	FRotator CurrentRotation = CenterMesh->GetRelativeRotation();

	// 새 회전값 계산
	FRotator NewRotation = CurrentRotation + (RotationSpeed * InDeltaTime);

	// 회전값 적용
	CenterMesh->SetRelativeRotation(NewRotation);
}

