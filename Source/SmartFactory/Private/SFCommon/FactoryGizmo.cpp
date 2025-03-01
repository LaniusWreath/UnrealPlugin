// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCommon/FactoryGizmo.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SFCommon/SFCLog.h"

// Sets default values
AFactoryGizmo::AFactoryGizmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StartPoint = FVector(0, 0, 0);
    EndPoint = FVector(100, 100, 100);
    CellSize = 1;
    FloorCount = 1;
    bShowDebugArea = false;
}


void AFactoryGizmo::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ShouldTickIfViewportsOnly();
}

bool AFactoryGizmo::ShouldTickIfViewportsOnly() const
{
    if (bShowDebugArea)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AFactoryGizmo::SetEnableDebugArea(bool inEnableDebugArea)
{
    bShowDebugArea = inEnableDebugArea;
    ShouldTickIfViewportsOnly();
}

void AFactoryGizmo::ShowDebugArea()
{
    FVector MinPoint = FVector(FMath::Min(StartPoint.X, EndPoint.X) - Offset.X * CellSize, FMath::Min(StartPoint.Y, EndPoint.Y) - Offset.Y * CellSize, 
        0 - Offset.Z * CellSize);
    FVector MaxPoint = FVector(FMath::Max(StartPoint.X, EndPoint.X) + Offset.X * CellSize, FMath::Max(StartPoint.Y, EndPoint.Y) + Offset.Y * CellSize,
        EndPoint.Z+ (UpperOffsetZ+Offset.Z) * CellSize * (FloorCount-1) + UpperOffsetZ*CellSize);

    FVector Center = (MinPoint + MaxPoint) / 2;
    FVector BoxExtent = (MaxPoint - MinPoint) / 2;

    DrawDebugBox(GetWorld(), Center, BoxExtent, FColor::Green, false, 0.1f, 0, 5.0f);
}


void AFactoryGizmo::GenerateFactoryLayout()
{
    UWorld* World = GetWorld();
    if (!World )
    {
        UE_LOG(SFClog, Error, TEXT("Couldn't find world"));
        return;
    }

    if (EdgeModules.Num() ==0 || CenterModules.Num() == 0|| CornerModules.Num() ==0)
    {
        UE_LOG(SFClog, Error, TEXT("Facoty Modules are invalid"));
        return;
    }

    FVector MinPoint = FVector(FMath::Min(StartPoint.X, EndPoint.X), FMath::Min(StartPoint.Y, EndPoint.Y), 0);
    FVector MaxPoint = FVector(FMath::Max(StartPoint.X, EndPoint.X), FMath::Max(StartPoint.Y, EndPoint.Y), 0);

    int32 XCount = (MaxPoint.X - MinPoint.X) / CellSize * 100;
    int32 YCount = (MaxPoint.Y - MinPoint.Y) / CellSize * 100;
    float FloorHeight = (Offset.Z+UpperOffsetZ+ 100) * CellSize ;

    // Place Corner Module

    for (int curFloor = 0; curFloor < FloorCount; curFloor++)
    {
        SpawnCornerAt(FVector(MinPoint.X, MinPoint.Y, FloorHeight * curFloor), FRotator(0, 0, 0), FVector(CellSize, CellSize, CellSize));  // 0,0,0
        SpawnCornerAt(FVector(MinPoint.X, MaxPoint.Y, FloorHeight * curFloor), FRotator(0, 270, 0), FVector(CellSize, CellSize, CellSize));  // 0,1,0
        SpawnCornerAt(FVector(MaxPoint.X, MinPoint.Y, FloorHeight * curFloor), FRotator(0, 90, 0), FVector(CellSize, CellSize, CellSize));  // 1,0,0
        SpawnCornerAt(FVector(MaxPoint.X, MaxPoint.Y, FloorHeight * curFloor), FRotator(0, 180, 0), FVector(CellSize, CellSize, CellSize));  // 1,1,0
    }
    
    
    // Place EdgeModule
    float CellUnitSize = CellSize * 100;
    int xEdgeCount = (MaxPoint.X - MinPoint.X) / CellUnitSize - 2;
    int yEdgeCount = (MaxPoint.Y - MinPoint.Y) / CellUnitSize - 2;

    float curX = 0;

    for (int curFloor = 0; curFloor < FloorCount; curFloor++)
    {
        for (int i = 1; i <= xEdgeCount; i++)
        {
            curX = MinPoint.X + CellUnitSize * i;
            SpawnEdgeAt(FVector(MaxPoint.X - curX, MinPoint.Y, FloorHeight * curFloor), FRotator(0, 90, 0), FVector(CellSize, CellSize, CellSize)); // Left
            SpawnEdgeAt(FVector(curX, MaxPoint.Y, FloorHeight * curFloor), FRotator(0, 270, 0), FVector(CellSize, CellSize, CellSize)); // Right
        }
    }

    float curY = 0;
    for (int curFloor = 0; curFloor < FloorCount; curFloor++)
    {
        for (int i = 1; i <= yEdgeCount; i++)
        {
            curY = MinPoint.Y + CellUnitSize * i;
            SpawnEdgeAt(FVector(MaxPoint.X, MaxPoint.Y - curY, FloorHeight * curFloor), FRotator(0, 180, 0), FVector(CellSize, CellSize, CellSize));  // Top
            SpawnEdgeAt(FVector(MinPoint.X, curY, FloorHeight * curFloor), FRotator(0, 0, 0), FVector(CellSize, CellSize, CellSize));  // Bottom
        }
    }
    

    // Place Center Module
    for (int curFloor = 0; curFloor < FloorCount; curFloor++)
    {
        for (int i = 1; i <= xEdgeCount; i++)
        {
            curX = MinPoint.X + CellUnitSize * i;

            for (int j = 1; j <= yEdgeCount; j++)
            {
                curY = MinPoint.Y + CellUnitSize * j;
                SpawnCenterAt(FVector(curX, curY, FloorHeight * curFloor), FVector(CellSize, CellSize, CellSize));
            }
        }
    }
}

void AFactoryGizmo::RemoveAndDestroyComponent()
{
    TArray<USceneComponent*> ChildComponents;
    RootComponent->GetChildrenComponents(true, ChildComponents);

    for (USceneComponent* Component : ChildComponents)
    {
        UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Component);
        if (StaticMeshComp)
        {
            // Delete from instance management list
            RemoveInstanceComponent(StaticMeshComp);

            // Delete Component
            StaticMeshComp->DestroyComponent();

            UE_LOG(LogTemp, Log, TEXT("Removed and destroyed component: %s"), *StaticMeshComp->GetName());
        }
    }

    MarkPackageDirty();
}

UStaticMesh* AFactoryGizmo::GetRandomMesh(const TArray<UStaticMesh*>& MeshArray)
{
    if (MeshArray.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, MeshArray.Num() - 1);
        return MeshArray[RandomIndex];
    }
    return nullptr;
}


// ---------------------------- Spawn Factory Static Mesh Module ----------------------------

void AFactoryGizmo::SpawnEdgeAt(const FVector& InLocation, const FRotator& InRotation, const FVector& InScale)
{
    if (EdgeModules.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("EdgeModule is invalid"));
        return;
    }

    UStaticMesh* SelectedMesh = GetRandomMesh(EdgeModules);
    if (!SelectedMesh)
    {
        return;
    }

    UStaticMeshComponent* WallComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
    WallComponent->SetStaticMesh(SelectedMesh);
    WallComponent->SetWorldLocation(InLocation);
    WallComponent->SetWorldRotation(InRotation);
    WallComponent->SetWorldScale3D(InScale);
    WallComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
    WallComponent->RegisterComponent();
    WallComponent->SetMobility(EComponentMobility::Static);

    AddInstanceComponent(WallComponent);

}

void AFactoryGizmo::SpawnCenterAt(const FVector& InLocation, const FVector& InScale)
{
    if (CenterModules.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CenterModule is invalid"));
        return;
    }

    UStaticMesh* SelectedMesh = GetRandomMesh(CenterModules);
    if (!SelectedMesh)
    {
        return;
    }

    UStaticMeshComponent* WallComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
    WallComponent->SetStaticMesh(SelectedMesh);
    WallComponent->SetWorldLocation(InLocation);
    WallComponent->SetWorldScale3D(InScale);
    WallComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
    WallComponent->RegisterComponent();
    WallComponent->SetMobility(EComponentMobility::Static);

    AddInstanceComponent(WallComponent);
}

void AFactoryGizmo::SpawnCornerAt(const FVector& InLocation, const FRotator& InRotation, const FVector& InScale)
{
    if (CornerModules.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CornerModule is invalid"));
        return;
    }

    UStaticMesh* SelectedMesh = GetRandomMesh(CornerModules);
    if (!SelectedMesh)
    {
        return;
    }

    UStaticMeshComponent* WallComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
    WallComponent->SetStaticMesh(SelectedMesh);
    WallComponent->SetWorldLocation(InLocation);
    WallComponent->SetWorldRotation(InRotation);
    WallComponent->SetWorldScale3D(InScale);
    WallComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
    WallComponent->RegisterComponent();
    WallComponent->SetMobility(EComponentMobility::Static);

    AddInstanceComponent(WallComponent);
}


// Called when the game starts or when spawned
void AFactoryGizmo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFactoryGizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bShowDebugArea)
    {
        ShowDebugArea();
    }
}

