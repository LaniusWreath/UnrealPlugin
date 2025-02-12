// Fill out your copyright notice in the Description page of Project Settings.


#include "Datas/JCMBarBaseActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/TextRenderComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BodyInstance.h"
#include "TimerManager.h"
#include "Datas/JCMLog.h"
#include "Datas/JCMChartGenerator.h"


// Sets default values
AJCMBarBaseActor::AJCMBarBaseActor()
{
	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultSceneRootComponent;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("BasicMesh"));
	ProcMeshComponent->SetupAttachment(RootComponent);

	CustomActorSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CustomMeshOffsetLayer"));
	CustomActorSceneComponent->SetupAttachment(RootComponent);

	CustomStaticMeshTemplateComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomMesh"));
	CustomStaticMeshTemplateComponent->SetupAttachment(RootComponent);

	//CustomStaticMeshTemplateComponent->SetHiddenInGame(true);

	// Procedural Mesh Component를 Navigation 시스템에서 제외. 경로탐색이나 ai상호작용이 필요 없는 경우, 꺼도 좋음. 
	// 안끄면 Navigation system에서 화냄. (계속 감시중)
	ProcMeshComponent->SetCanEverAffectNavigation(false);
	BarAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BarAnimationTimeline"));

	// 텍스트 렌더러 - 값
	TextRenderComponentValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentValue"));
	TextRenderComponentValue->SetupAttachment(RootComponent);

	// 텍스트 피벗을 가운데로 설정
	TextRenderComponentValue->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentValue->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	
	// 텍스트 렌더러 - 라벨
	TextRenderComponentLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentLabel")); 
	TextRenderComponentLabel->SetupAttachment(RootComponent);

	TextRenderComponentLabel->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRenderComponentLabel->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

}


// Called when the game starts or when spawned
void AJCMBarBaseActor::BeginPlay()
{
	Super::BeginPlay();

	// 템플릿으로 둔 커스텀 스태틱 메시의 콜리전 제거
	CustomStaticMeshTemplateComponent->SetSimulatePhysics(false);
	CustomStaticMeshTemplateComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 템플릿으로 둔 커스텀 스태틱 메시는 에디터 내에서 표시/숨김 제어해야 함.

}

void AJCMBarBaseActor::CreateProceduralBoxMesh(float BarHeight)
{
	// 필요한 배열 선언
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> Tangents;

	float BarWidth = Width_bar;
	float w = BarWidth / 2;

	// 박스의 크기 설정 (박스 너비)
	FVector BoxRadius(w, w, w); // 박스의 절반 크기

	// 박스 메쉬 생성
	UKismetProceduralMeshLibrary::GenerateBoxMesh(BoxRadius, Vertices, Triangles, Normals, UV0, Tangents);

	// 박스 높이만큼 스케일 계산해서 높이 올림.
	float zScaler = BarHeight / (BarWidth);
	//UE_LOG(LogTemp, Log, TEXT("BarBaseActor : zScaler is %f"), zScaler);
	ProcMeshComponent->SetWorldScale3D(FVector(1.f, 1.f, zScaler));
	ProcMeshComponent->AddWorldOffset(FVector(0, 0, (BarHeight / 2)));

	// Procedural Mesh Component에 메시 생성
	ProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, {}, Tangents, true);

	if (MeshMaterial)
	{
		ProcMeshComponent->SetMaterial(0, MeshMaterial);
	}
}

// 커스텀 스태틱 메쉬 생성 루틴 : 개수 자동 계산
void AJCMBarBaseActor::CreateCustomMeshRoutine(float BarHeight)
{
	// 커스텀 메시 유닛 높이 : 유닛 높이 * 로컬 스케일러
	float UnitMeshHeight = GetStaticMeshBoxUnitSize(CustomStaticMeshTemplateComponent->GetStaticMesh()).Z * 
		CustomStaticMeshTemplateComponent->GetRelativeScale3D().Z;

	// 생성해야하는 메시 개수 : 
	int32 UnitMeshAmount;
	UnitMeshAmount = BarHeight / UnitMeshHeight;

	// 타이머 실행, 람다로 매개변수 있는 함수 캡쳐
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [this, BarHeight, UnitMeshHeight, UnitMeshAmount]()
	{
		CreateSingleCustomMeshComponent(BarHeight, UnitMeshHeight, UnitMeshAmount); 
	}, CustomMeshSpawnWaitingTime, true);
}

// 단위만큼 나눠 차트 메쉬 생성
void AJCMBarBaseActor::CreateCustomMeshRoutine(float BarHeight, int32 amount)
{
	// 커스텀 메시 유닛 높이 : 유닛 높이 * 로컬 스케일러
	float UnitMeshHeight = GetCustomMeshUnitHeight();

	// 생성해야하는 메시 개수 : 
	int32 UnitMeshAmount = amount;

	// 타이머 실행, 람다로 매개변수 있는 함수 캡쳐
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [this, BarHeight, UnitMeshHeight, UnitMeshAmount]()
		{
			CreateSingleCustomMeshComponent(BarHeight, UnitMeshHeight, UnitMeshAmount);
		}, CustomMeshSpawnWaitingTime, true);
}

// 하나만 생성하는 루틴 함수
void AJCMBarBaseActor::CreateCustomMeshRoutine()
{
	// 커스텀 메시 유닛 높이 : 유닛 높이 * 로컬 스케일러
	float UnitMeshHeight = GetCustomMeshUnitHeight();

	CreateSingleCustomMeshComponent(UnitMeshHeight);
}

// 박스형태 메시 유닛 사이즈 리턴
FVector AJCMBarBaseActor::GetStaticMeshBoxUnitSize(UStaticMesh* TargetStaticMesh) const
{
	FVector BoundsExtent = TargetStaticMesh->GetBounds().BoxExtent;
	return BoundsExtent * 2.0f;
}

// 커스텀 스태틱 메쉬 생성 함수
void AJCMBarBaseActor::CreateSingleCustomMeshComponent(const float BarHeight, const float UnitMeshHeight, 
	const int32 SpawnAmount)
{
	// 스폰 카운트 제한 체크, 
	if (SpawnCount >= SpawnAmount)
	{
		ClearSpawnTimerHandle();
	}
	else
	{
		// StaticMeshComponent를 동적으로 생성하고, 부모 액터에 속하도록 설정
		UStaticMeshComponent* UnitMeshComponent = NewObject<UStaticMeshComponent>(this);

		// 템플릿 스태틱 메쉬 컴포넌트로부터 메쉬, 머티리얼, 스케일 복사
		InitializeStaticMeshProperty(UnitMeshComponent, CustomStaticMeshTemplateComponent);
		

		if (bEnablePhysics)
		{
			// 템플릿으로부터 피직스 복사
			InitializeCustomStaticMeshPhysics(UnitMeshComponent, CustomStaticMeshTemplateComponent);
		}
		
		// 부착
		UnitMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

		// Z축 스폰 오프셋 조정
		if (SpawnPerUnitValue)
		{
			UnitMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight * SpawnAmount + UnitMeshHeight/2));
		}
		else 
		{
			UnitMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));
		}

		// 월드에 컴포넌트를 등록하여 액터와 함께 관리되도록 설정 
		UnitMeshComponent->RegisterComponent();

		// 스폰 카운트
		SpawnCount++;
	}
}

// 메시 하나만 생성하는 함수
void AJCMBarBaseActor::CreateSingleCustomMeshComponent(const float UnitMeshHeight)
{
	// StaticMeshComponent를 동적으로 생성하고, 부모 액터에 속하도록 설정
	UStaticMeshComponent* UnitMeshComponent = NewObject<UStaticMeshComponent>(this);

	// 인벤토리가 아닌 템플릿 스태틱 메쉬 컴포넌트로부터 메쉬, 머티리얼, 스케일 복사
	InitializeStaticMeshProperty(UnitMeshComponent, CustomStaticMeshTemplateComponent);
	
	if (bEnablePhysics)
	{
		// 템플릿으로부터 피직스 복사
		InitializeCustomStaticMeshPhysics(UnitMeshComponent, CustomStaticMeshTemplateComponent);
	}

	// 부착
	UnitMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z축 스폰 오프셋 조정
	UnitMeshComponent->AddWorldOffset(FVector(0, 0, UnitMeshHeight / 2));

	UnitMeshComponent->RegisterComponent();
}

// 기존 생성한 스태틱 메쉬 컴포넌트 삭제
void AJCMBarBaseActor::ClearCustomMeshes()
{
	if (CustomActorSceneComponent->GetNumChildrenComponents() > 0)
	{
		TArray<USceneComponent*> AttachedComponents;
		CustomActorSceneComponent->GetChildrenComponents(false, AttachedComponents);

		for (USceneComponent* ChildComponent : AttachedComponents)
		{
			if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(ChildComponent))
			{
				MeshComponent->DestroyComponent();
			}
		}
		UE_LOG(JCMlog, Log, TEXT("%s : All customStaticmesh components cleard"), *this->GetAttachParentActor()->GetName());
	}
}

// 메쉬 스폰 타이머 핸들 초기화 : BarBaseActor는 BarGenerator에서 Gernating 될 때 마다 새로운 인스턴스가 생성되어 기능함.
// 따라서 기존 BarBaseActor의 메쉬 생성 타이머를 정지시키고 싶을 때는 인스턴스를 소유하고 있는 BarGenerator에서 직접 시켜줘야 함.
void AJCMBarBaseActor::ClearSpawnTimerHandle()
{
	if (GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		// 타이머 종료
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		// 타이머 종료시 호출할 함수는 여기에 작성
	}
}

void AJCMBarBaseActor::InitializeStaticMeshProperty(UStaticMeshComponent* TargetStaticMeshComponent, 
	const UStaticMeshComponent* TemplateMeshComponent)
{
	// 템플릿의 속성을 UnitMeshComponent에 복사
	TargetStaticMeshComponent->SetStaticMesh(TemplateMeshComponent->GetStaticMesh());
	TargetStaticMeshComponent->SetMaterial(0, TemplateMeshComponent->GetMaterial(0));
	TargetStaticMeshComponent->SetRelativeScale3D(TemplateMeshComponent->GetRelativeScale3D());
	TargetStaticMeshComponent->SetCastShadow(false);
}

// 단위로 나누고 나머지 남은 높이, 스케일링 된 유닛 상자 만들어 스폰 : 현재는 안씀
void AJCMBarBaseActor::CreateAdditionalCustomMeshComponent(float BarHeight, float RestHeight, float UnitMeshHeight)
{	
	// StaticMeshComponent를 동적으로 생성하고, 부모 액터에 속하도록 설정
	UStaticMeshComponent* RestMeshComponent = NewObject<UStaticMeshComponent>(this);

	// 물리 형질 복사
	InitializeCustomStaticMeshPhysics(RestMeshComponent, CustomStaticMeshTemplateComponent);

	// 스케일 조정
	float scaler = RestHeight / UnitMeshHeight;
	RestMeshComponent->SetRelativeScale3D(RestMeshComponent->GetRelativeScale3D() * FVector(1, 1, scaler));

	// 부착
	RestMeshComponent->AttachToComponent(CustomActorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Z축 스폰 오프셋 조정
	RestMeshComponent->AddWorldOffset(FVector(0, 0, BarHeight + UnitMeshHeight * 3));

	// 월드에 컴포넌트를 등록하여 액터와 함께 관리되도록 설정 
	RestMeshComponent->RegisterComponent();
}

// 템플릿 메쉬로부터 속성 복사
void AJCMBarBaseActor::InitializeCustomStaticMeshPhysics(UStaticMeshComponent* TargetStaticMesh, UStaticMeshComponent* TemplateComponent)
{
	// 피직스 복사 : BodyInstance를 직접 복사할 수 있지만, 그 경우에는 AttachToComponent를 사용할 수 없음. -> 개별 복사 필요
	TargetStaticMesh->SetSimulatePhysics(true); // 시뮬레이션 여부 복사
	TargetStaticMesh->SetEnableGravity(TemplateComponent->IsGravityEnabled()); // 중력 on
	TargetStaticMesh->SetLinearDamping(TemplateComponent->GetLinearDamping()); // 선형 댐핑
	TargetStaticMesh->SetAngularDamping(TemplateComponent->GetAngularDamping()); // 회전각 댐핑
	//UnitMeshComponent->SetPhysMaterialOverride(CustomStaticMeshComponent->GetBodySetup()->GetPhysMaterial()); // 피지컬 머티리얼
	// 피지컬 머티리얼은 자원 많이 쓰고, 복사하는데도 많은 자원 필요하니, 안함
	TargetStaticMesh->BodyInstance.SetMassOverride(TemplateComponent->BodyInstance.GetMassOverride()); //질량

	// 콜리전 복사 : 콜리전-피직스는 bodyinstance로 묶어 관리 됨. bodyinstance 사용 못하는 현재, 콜리전도 일일이 복사
	TargetStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetStaticMesh->SetCollisionObjectType(TemplateComponent->GetCollisionObjectType());
	TargetStaticMesh->SetCollisionResponseToChannels(TemplateComponent->GetCollisionResponseToChannels());

	// 컨스트레인트 복사
	TargetStaticMesh->BodyInstance.bLockRotation = TemplateComponent->BodyInstance.bLockRotation;
	TargetStaticMesh->BodyInstance.bLockTranslation = TemplateComponent->BodyInstance.bLockTranslation;

	// 충격량 반영 여부
	TargetStaticMesh->bApplyImpulseOnDamage = TemplateComponent->bApplyImpulseOnDamage;
}

void AJCMBarBaseActor::AdjustTextMeshComponentOffsetRoutine(const float InHeight)
{
	AdjustTextMeshValueOffset(TextRenderComponentValue, InHeight); // 텍스트 렌더 컴포넌트 오프셋 조정
	AdjustTextMeshOffset(TextRenderComponentLabel);
}


// 에디터 상에 Procedural Mesh 또는 커스텀 메시 생성 유무 bool로 추출해놓음 분기하여 메시 생성 함수 결정
void AJCMBarBaseActor::CreateMesh(float BarHeight, int Value)
{
	// 프로시저럴 메쉬
	if (!bEnableSpawnCustomMesh)
	{
		CreateProceduralBoxMesh(BarHeight);	// 메쉬 생성
		AdjustTextMeshComponentOffsetRoutine(BarHeight); // 텍스트 렌더 컴포넌트 오프셋 조정 함수들 호출
	}
	// 커스텀 메쉬
	else
	{
		if (!CustomStaticMeshTemplateComponent)
		{
			UE_LOG(JCMlog, Error, TEXT("CustomStaticMeshTemplateComponent is invalid"));
			return;
		}
		float UnitMeshHeight = GetCustomMeshUnitHeight();
		// 단위 지정하여 메쉬 생성
		if (SpawnPerUnitValue)
		{
			// 숨겨진 기능! UnitValue가 0이면 무조건 하나만 생성
			if (UnitValue == 0)
			{
				SpawnedCustomMeshAmount = 1;
				CreateCustomMeshRoutine();
			}
			else
			{
				// 사용자 정의 단위로 나눠 
				SpawnedCustomMeshAmount = int32(Value / UnitValue);
				CreateCustomMeshRoutine(BarHeight, SpawnedCustomMeshAmount);
			}
		}
		// 개수 자동 계산하여 메쉬 생성
		else
		{
			// 생성해야하는 메시 개수 : 
			SpawnedCustomMeshAmount = int32(BarHeight / UnitMeshHeight);
			CreateCustomMeshRoutine(BarHeight);
		}

		// 데이터 숫자값 표시 컴포넌트 오프셋 조정 함수들 호출
		AdjustTextMeshComponentOffsetRoutine(SpawnedCustomMeshAmount * UnitMeshHeight);
	}
}

// 라벨 텍스트 렌더러 설정
void AJCMBarBaseActor::InitializeTextMeshLabel(const FString& LabelName)
{
	// 텍스트 내용
	TextRenderComponentLabel->SetText(FText::FromString(LabelName));
}

// 값 텍스트 렌더러 설정
void AJCMBarBaseActor::InitializeTextMeshValue(const float& FloatValue)
{
	// 텍스트 내용
	TextRenderComponentValue->SetText(FText::AsNumber(FloatValue));
}

// 정확한 높이에 텍스트 렌더 컴포넌트 배치
void AJCMBarBaseActor::AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const float& BarHeight)
{
	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, BarHeight + (TargetTextRednerComponent->WorldSize) / 2
			+ TextRenderComponentOffset_Value));
}

// 커스텀메쉬 사이즈 단위로 나눈 높이에 텍스트 렌더 컴포넌트 배치
void AJCMBarBaseActor::AdjustTextMeshValueOffset(UTextRenderComponent* TargetTextRednerComponent, const int32& amount)
{
	// 커스텀 메시 유닛 높이 : 유닛 높이 * 로컬 스케일러
	float UnitMeshHeight = GetCustomMeshUnitHeight();

	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, (UnitMeshHeight*amount + (TargetTextRednerComponent->WorldSize / 2)
		+ TextRenderComponentOffset_Value)));
}

// 추가 높이 조정 없이 텍스트 
void AJCMBarBaseActor::AdjustTextMeshOffset(UTextRenderComponent* TargetTextRednerComponent)
{
	TargetTextRednerComponent->AddWorldOffset(FVector(0.f, 0.f, (TargetTextRednerComponent->WorldSize / 2)
		+ TextRenderComponentOffset_Value));
}

// 차트 생성 종료시 바인딩할 델리게이트 함수 
void AJCMBarBaseActor::OnChartGeneratingDoneBindingRoutine()
{
	return;
}

void AJCMBarBaseActor::OnChartSearchingBindingRoutine(int32 InIndex)
{
	return;
}

void AJCMBarBaseActor::BindToChartSearching(FChartSearchingDelegate& Delegate)
{
	Delegate.AddUObject(this, &AJCMBarBaseActor::OnChartSearchingBindingRoutine);
}

void AJCMBarBaseActor::BindToChartGeneratingEnd(FChartGeneratingDoneDelegate& Delegate)
{
	Delegate.BindUObject(this, &AJCMBarBaseActor::OnChartGeneratingDoneBindingRoutine);
}

// 애니메이션 제어
void AJCMBarBaseActor::BindTimelineAnimation()
{
	// 애니메이션 초기화
	if (AnimationCurve && !bEnableSpawnCustomMesh)
	{
		FOnTimelineFloat TimelineCallBack;
		// 타임라인에 함수 바인딩
		TimelineCallBack.BindUFunction(this, FName("OnAnimationUpdate"));

		// Timeline에 Curve와 Curve를 사용할 Callback 함수 추가
		BarAnimationTimeline->AddInterpFloat(AnimationCurve, TimelineCallBack);
		BarAnimationTimeline->SetLooping(false);
		BarAnimationTimeline->Play();
	}
	else
	{
		if (!bEnableSpawnCustomMesh)
		{
			UE_LOG(JCMlog, Warning, TEXT("%s : Failed finding animation cuvrve"), *this->GetAttachParentActor()->GetName());
		}
	}
}

float AJCMBarBaseActor::GetCustomMeshUnitHeight()
{
	if (CustomStaticMeshTemplateComponent)
	{
		return GetStaticMeshBoxUnitSize(CustomStaticMeshTemplateComponent->GetStaticMesh()).Z *
			CustomStaticMeshTemplateComponent->GetRelativeScale3D().Z;
	}	
	else
	{
		UE_LOG(JCMlog, Warning, TEXT("%s : CustomStaticMesh is invalid"), *this->GetAttachParentActor()->GetName());
		return 1;
	}
}

// 애니메이션 실행 제어
void AJCMBarBaseActor::PlayBarAnimation()
{
	if (bEnableSpawnCustomMesh)
	{
		return;
	}
	BarAnimationTimeline->PlayFromStart();
}

// 막대 스케일 변경 애니메이션 실행
void AJCMBarBaseActor::OnAnimationUpdate(float Value)
{
	FVector CurrentScale = GetActorScale();
	SetActorScale3D(FVector(CurrentScale.X, CurrentScale.Y, Value));
}

void AJCMBarBaseActor::SetParentSplineIndex(const int32 InIndex)
{
	ParentSplineIndex = InIndex;
}

// 텍스트 렌더 컴포넌트로부터 숫자 값 int32로 가져오는 함수
int32 AJCMBarBaseActor::GetValueFromTextRenderComponent(UTextRenderComponent* TargetTextRenderComponent)
{
	if (TargetTextRenderComponent->Text.IsEmpty())
	{
		UE_LOG(JCMlog, Warning, TEXT("SafeValue is Empty"));
		return 0;
	}

	FText ValueText = TargetTextRenderComponent->Text;
	FString ValueString = ValueText.ToString();

	if (!ValueString.IsNumeric()) // 문자열이 숫자인지 확인
	{
		UE_LOG(JCMlog, Warning, TEXT("SafeValue is not number"));
		return 0;
	}

	int32 Value = FCString::Atoi(*ValueString); // FString을 int32로 변환
	return Value;
}

// 스플라인 인덱스에 해당하는 생성된 액터의 생성된 스태틱 메쉬 컴포넌트 가져옴
UStaticMeshComponent* AJCMBarBaseActor::GetCustomStaticMeshComponent(const int32 SplineIndex)
{
	// RootSceneComponent의 자식 컴포넌트 가져오기
	USceneComponent* AttachedChildComponent = CustomActorSceneComponent->GetChildComponent(SplineIndex);

	// UStaticMeshComponent인지 확인
	if (UStaticMeshComponent* TargetStaticMeshComponent = Cast<UStaticMeshComponent>(AttachedChildComponent))
	{
		return TargetStaticMeshComponent;
	}
	else
	{
		UE_LOG(JCMlog, Error, TEXT("Invalid CustomActorSceneComponent Index"))
		return nullptr;
	}
}

