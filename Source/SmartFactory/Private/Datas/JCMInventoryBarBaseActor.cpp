// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMInventoryBarBaseActor.h"
#include "Datas/JCMLog.h"
#include "Components/TextRenderComponent.h"
#include "Datas/StaticMeshInventory.h"


AJCMInventoryBarBaseActor::AJCMInventoryBarBaseActor()
{

	// 아이템 메쉬 컴포넌트 초기화
	TemplateItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InventoryItemStaticMeshComponent"));
	TemplateItemMeshComponent->SetupAttachment(CustomStaticMeshTemplateComponent);

	// 텍스트 렌더러 - 값
	TextRenderComponentSafeValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentMaxValue"));
	TextRenderComponentSafeValue->SetupAttachment(RootComponent);

	// 텍스트 피벗을 가운데로 설정
	TextRenderComponentSafeValue->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentSafeValue->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

}

// 입력받은 스태틱 메쉬 컴포넌트에 인벤토리 메쉬 적용 : 안씀.
void AJCMInventoryBarBaseActor::InitializeStaticMeshPropertyFromInventory(UStaticMeshComponent* TargetStaticMeshComponent, const int32 InInventoryIndex)
{
	UStaticMesh* TemplateStaticMesh = GetStaticMeshFromInventory(InInventoryIndex);

	if (!TemplateStaticMesh)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Invalid StaticMesh"), *this->GetName());
		return;
	}

	TargetStaticMeshComponent->SetStaticMesh(TemplateStaticMesh);
}

// 입력받은 텍스트 렌더 컴포넌트에 인벤토리 MaxValue 적용
void AJCMInventoryBarBaseActor::InitializeTextRenderComponentMaxValue(UTextRenderComponent* TargetTextRenderComponent,
	const int32 InInventoryIndex)
{
	if (!InventoryDataAsset)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory Data Asset is invalid"), *this->GetName());
		return;
	}

	int32 Amount = InventoryDataAsset->GetAmount(InInventoryIndex);

	TargetTextRenderComponent->SetText(FText::AsNumber(Amount));

	// 델리게이트 바인딩

}

// 델리게이트에 바인딩 할 함수 묶음
void AJCMInventoryBarBaseActor::OnChartGeneratingDoneBindingRoutine()
{
	AlarmSafeAmount();

}

void AJCMInventoryBarBaseActor::OnChartSearchingBindingRoutine(int32 InIndex)
{
	ChangeSearchedComponentMaterial(InIndex);
}

void AJCMInventoryBarBaseActor::ChangeSearchedComponentMaterial(const int32 InIndex)
{
	// 첫 번째 생성된 스태틱 메쉬 컴포넌트 참조
	UStaticMeshComponent* TargetStaticMeshComponent = GetCustomStaticMeshComponent(0);
	if (!TargetStaticMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("ChangeSearchedComponentMaterial : couldn't find GeneratedStaticMeshComponent"));
		return;
	}

	// 검색 대상이 되었을 때
	if (InIndex == ParentSplineIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeSearchedComponentMaterial : index = %d, current : %d : searching"), InIndex, ParentSplineIndex);
		ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, SearchedMaterial);
		if (CurrentMaterialState != EMaterialState::SEARCH)
		{
			PreMaterialState = CurrentMaterialState;
		}
		CurrentMaterialState = EMaterialState::SEARCH;
	}
	else
	{
		// 이전에 검색 대상이었지만 아니게 되었을 때
		if (CurrentMaterialState == EMaterialState::SEARCH)
		{
			CurrentMaterialState = PreMaterialState;
			if (CurrentMaterialState == EMaterialState::DEFAULT)
			{
				UE_LOG(LogTemp, Warning, TEXT("ChangeSearchedComponentMaterial : index = %d, current : %d : back to default"), InIndex, ParentSplineIndex);
				ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, DefaultMaterial);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ChangeSearchedComponentMaterial : index = %d, current : %d : back to alarm"), InIndex, ParentSplineIndex);
				ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, AlarmMaterial);
			}
		}
		else
		{
			// 이도저도 아닐때
			UE_LOG(LogTemp, Warning, TEXT("ChangeSearchedComponentMaterial : index = %d, current : %d : nothing"), InIndex, ParentSplineIndex);
			return;
		}
	}
}

// 알람 기능 함수 묶음
void AJCMInventoryBarBaseActor::AlarmSafeAmount()
{
	int32 SafeAmount = GetValueFromTextRenderComponent(TextRenderComponentSafeValue);
	int32 CurrentAmount = GetValueFromTextRenderComponent(TextRenderComponentValue);

	// 첫 번째 생성된 스태틱 메쉬 컴포넌트 참조
	UStaticMeshComponent* TargetStaticMeshComponent = GetCustomStaticMeshComponent(0);
	if (!TargetStaticMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("SafeAmountAlarmRoutine : couldn't find GeneratedStaticMeshComponent"));
		return;
	}

	// 여유 수량보다 많으면 원래 색으로 변경
	if (CurrentAmount > SafeAmount)
	{
		// 머티리얼 기본으로 변경
		if (!DefaultMaterial)
		{
			UE_LOG(JCMlog, Error, TEXT("SafeAmountAlarmRoutine : couldn't find DefaultMaterial"));
			return;
		}
		// 머티리얼 기본으로 변경
		ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, DefaultMaterial);
		CurrentMaterialState = EMaterialState::DEFAULT;
		// 텍스트 색상 변경
		TextRenderComponentValue->SetTextRenderColor(FColor::White);

		return;
	}
	else
	{
		if (!AlarmMaterial)
		{
			UE_LOG(JCMlog, Error, TEXT("SafeAmountAlarmRoutine : couldn't find AlarmMaterial"));
			return;
		}

		// 머티리얼 알람용으로 변경
		ChangeStaticMeshComponentMaterial(TargetStaticMeshComponent, AlarmMaterial);
		CurrentMaterialState = EMaterialState::ALARM;
		// 텍스트 색상 변경
		TextRenderComponentValue->SetTextRenderColor(FColor::Red);
	}
}

UStaticMeshComponent* AJCMInventoryBarBaseActor::ChangeStaticMeshComponentMaterial(UStaticMeshComponent* TargetStaticMeshComponent,
	UMaterialInterface* InMaterial)
{
	if (!TargetStaticMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("ChangeStaticMeshComponentMaterial : TargetStaticMeshComponent is invalid"));
		return nullptr;
	}
	if (!InMaterial)
	{
		UE_LOG(JCMlog, Error, TEXT("ChangeStaticMeshComponentMaterial : InMaterial is invalid"));
		return nullptr;
	}
	
	// 이부분 머티리얼 0번 슬롯만 바꾸도록 하드코딩 했는데, 오류 생길 수 있음.
	TargetStaticMeshComponent->SetMaterial(0, InMaterial);
	return nullptr;
}

// 메시 컴포넌트 재생성 없이 숫자 데이터만 직접 접근하여 업데이트
void AJCMInventoryBarBaseActor::UpdateData(const int CurrentAmount, const int SafeAmount, const FString& ItemLabel)
{
	TextRenderComponentValue->SetText(FText::AsNumber(CurrentAmount));
	TextRenderComponentSafeValue->SetText(FText::AsNumber(SafeAmount));
	TextRenderComponentLabel->SetText(FText::FromString(ItemLabel));
}

void AJCMInventoryBarBaseActor::OnSearched()
{
}



// 인벤토리에서 스태틱 메쉬 가져오기
UStaticMesh* AJCMInventoryBarBaseActor::GetStaticMeshFromInventory(const int32 InInventoryIndex)
{
	if (!InventoryDataAsset)
	{
		UE_LOG(JCMlog, Error, TEXT("%s : Inventory Data Asset is invalid"), *this->GetName());
		return nullptr;
	}

	if (!InventoryDataAsset->GetStaticMesh(InInventoryIndex))
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : Inventory member is invalid"), *this->GetName());
		// 유효하지 않으면, 템플릿 스태틱 메쉬 사용
		return TemplateItemMeshComponent->GetStaticMesh();
	}

	return InventoryDataAsset->GetStaticMesh(InInventoryIndex);
}

// 아이템 메시 초기화
void AJCMInventoryBarBaseActor::InitializeItemStaticMeshPrepertyFromTemplate(UStaticMeshComponent* TargetStaticMeshComponent,
	const int32 InventoryIndex)
{
	if (!TemplateItemMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("TemplateItemStaticMeshComponent is Invalid"));
		return;
	}

	if (!TargetStaticMeshComponent)
	{
		UE_LOG(JCMlog, Error, TEXT("TargetStaticMeshComponent is Invalid"));
		return;
	}

	// 스태틱 메쉬 적용
	TargetStaticMeshComponent->SetStaticMesh(GetStaticMeshFromInventory(InventoryIndex));

	// 스태틱 메쉬 스케일링
	ScaleStaticMeshToTemplateBounds(TargetStaticMeshComponent, CustomStaticMeshTemplateComponent);

	// 섀도우
	TargetStaticMeshComponent->SetCastShadow(false);
}

// 개수 만큼 커스텀 메쉬 컴포넌트 생성
void AJCMInventoryBarBaseActor::CreateSingleCustomMeshComponent(const float BarHeight, const float UnitMeshHeight, int32 SpawnAmount)
{
	// 스폰 카운트 제한 체크, 
	if (SpawnCount >= SpawnAmount)
	{
		ClearSpawnTimerHandle();
	}
	else
	{
		// 커버 스태틱 메쉬 컴포넌트를 동적으로 생성하고, 부모 액터에 속하도록 설정
		UStaticMeshComponent* CoverMeshComponent = NewObject<UStaticMeshComponent>(this);

		// 아이템 스태틱 메쉬 컴포넌트를 동적으로 생성하고, 부모 액터에 속하도록 설정
		UStaticMeshComponent* ItemMeshComponent = NewObject<UStaticMeshComponent>(this);

		// 템플릿으로부터 커버 메쉬, 머티리얼, 스케일 복사
		InitializeStaticMeshProperty(CoverMeshComponent, CustomStaticMeshTemplateComponent);

		// 템플릿으로부터 아이템 메쉬, 스케일 초기화
		InitializeItemStaticMeshPrepertyFromTemplate(ItemMeshComponent, SpawnCount);

		// 데이터 애셋으로부터 최대 수량 가져와 텍스트 변경
		InitializeTextRenderComponentMaxValue(TextRenderComponentSafeValue, SpawnCount);

		if (bEnablePhysics)
		{
			// 템플릿으로부터 피직스 복사
			InitializeCustomStaticMeshPhysics(CoverMeshComponent, CustomStaticMeshTemplateComponent);
		}

		// 아이템 커버에 부착
		ItemMeshComponent->AttachToComponent(CoverMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);

		CoverMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

		// Z축 스폰 오프셋 조정
		if (SpawnPerUnitValue)
		{
			CoverMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight * SpawnAmount + UnitMeshHeight / 2));
		}
		else
		{
			CoverMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));
		}

		// 월드에 컴포넌트를 등록하여 액터와 함께 관리되도록 설정 
		ItemMeshComponent->RegisterComponent();
		CoverMeshComponent->RegisterComponent();

		// 스폰 카운트
		SpawnCount++;

		// 회전 애니메이션 바인딩
		InitializeItemMeshRotation(ItemMeshComponent, RotationSpeed);
	}
}

// 스태틱 메쉬 컴포넌트 하나만 생성
void AJCMInventoryBarBaseActor::CreateSingleCustomMeshComponent(const float UnitMeshHeight)
{
	// StaticMeshComponent를 동적으로 생성하고, 부모 액터에 속하도록 설정
	UStaticMeshComponent* CoverMeshComponent = NewObject<UStaticMeshComponent>(this);

	// 아이템 스태틱 메쉬 컴포넌트를 동적으로 생성하고, 부모 액터에 속하도록 설정
	UStaticMeshComponent* ItemMeshComponent = NewObject<UStaticMeshComponent>(this);

	// 템플릿으로부터 커버 메쉬, 머티리얼, 스케일 복사
	InitializeStaticMeshProperty(CoverMeshComponent, CustomStaticMeshTemplateComponent);

	// 템플릿으로부터 아이템 메쉬, 스케일 초기화
	InitializeItemStaticMeshPrepertyFromTemplate(ItemMeshComponent, ParentSplineIndex);

	// 데이터 애셋으로부터 최대 수량 가져와 텍스트 변경
	InitializeTextRenderComponentMaxValue(TextRenderComponentSafeValue, ParentSplineIndex);

	if (bEnablePhysics)
	{
		// 템플릿으로부터 피직스 복사
		InitializeCustomStaticMeshPhysics(CoverMeshComponent, CustomStaticMeshTemplateComponent);
	}

	// 아이템 커버에 부착
	ItemMeshComponent->AttachToComponent(CoverMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// 커버 루트에 부착
	CoverMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z축 스폰 오프셋 조정
	CoverMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight / 2));

	CoverMeshComponent->RegisterComponent();
	ItemMeshComponent->RegisterComponent();

	// 회전 애니메이션 바인딩
	InitializeItemMeshRotation(ItemMeshComponent, RotationSpeed);
}


// 템플릿 스태틱 메쉬 컴포넌트의 바운딩 크기 만큼 타겟 컴포넌트 스케일링
void AJCMInventoryBarBaseActor::ScaleStaticMeshToTemplateBounds(UStaticMeshComponent* NewMesh, UStaticMeshComponent* TemplateMesh)
{
	if (!NewMesh || !NewMesh->GetStaticMesh() || !TemplateMesh || !TemplateMesh->GetStaticMesh())
	{
		UE_LOG(JCMlog, Warning, TEXT("Invalid Mesh Components"));
		return;
	}

	// 템플릿 메쉬 바운드
	FBoxSphereBounds TemplateBounds = TemplateMesh->GetStaticMesh()->GetBounds();

	// 새 컴포넌트의 바운드
	FBoxSphereBounds NewBounds = NewMesh->GetStaticMesh()->GetBounds();

	// 템플릿과 타겟 컴포넌트의 최대 크기 비교
	float TemplateMaxSize = FMath::Max3(TemplateBounds.BoxExtent.X, TemplateBounds.BoxExtent.Y, TemplateBounds.BoxExtent.Z) * 2.0f;
	float NewMaxSize = FMath::Max3(NewBounds.BoxExtent.X, NewBounds.BoxExtent.Y, NewBounds.BoxExtent.Z) * 2.0f;

	// 크기 비교 계산
	if (NewMaxSize > TemplateMaxSize)
	{
		float ScaleFactor = TemplateMaxSize / NewMaxSize;

		// 템플릿 스케일 값만큼 추가 조정
		ScaleFactor *= TemplateMesh->GetRelativeScale3D().X;
		NewMesh->SetRelativeScale3D(FVector(ScaleFactor));
	}
}

void AJCMInventoryBarBaseActor::InitializeItemMeshRotation(UStaticMeshComponent* TargetStaticMeshComponent, 
	const float InRotationSpeed)
{
	// 기존 타이머 제거 (초기화)
	GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);

	// 타이머를 사용해 회전 함수 호출
	GetWorld()->GetTimerManager().SetTimer(RotationTimerHandle, FTimerDelegate::CreateLambda([TargetStaticMeshComponent, 
		InRotationSpeed]()
		{
			if (TargetStaticMeshComponent)
			{
				// 기존 회전
				FRotator CurrentRotation = TargetStaticMeshComponent->GetRelativeRotation();

				// 회전 추가
				CurrentRotation.Yaw += InRotationSpeed;

				TargetStaticMeshComponent->SetRelativeRotation(CurrentRotation);
			}
		}), 0.1f, true);
}

void AJCMInventoryBarBaseActor::AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const float& BarHeight)
{
	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, TargetTextRednerComponent->WorldSize / 2
		+ TextRenderComponentOffset_Value + GetCustomMeshUnitHeight()/2));
}

void AJCMInventoryBarBaseActor::AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const int32& amount)
{
	// 커스텀 메시 유닛 높이 : 유닛 높이 * 로컬 스케일러
	float UnitMeshHeight = GetCustomMeshUnitHeight();

	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, TargetTextRednerComponent->WorldSize / 2
		+ TextRenderComponentOffset_Value + GetCustomMeshUnitHeight()/2));
}

void AJCMInventoryBarBaseActor::AdjustTextMeshComponentOffsetRoutine(const float InHeight)
{
	AdjustTextMeshValueOffset(TextRenderComponentValue, InHeight); // 텍스트 렌더 컴포넌트 오프셋 조정
	AdjustTextMeshOffset(TextRenderComponentLabel);
	AdjustTextMeshOffset(TextRenderComponentSafeValue);
}