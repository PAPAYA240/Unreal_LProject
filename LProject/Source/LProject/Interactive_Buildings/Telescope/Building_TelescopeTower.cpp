// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive_Buildings/Telescope/Building_TelescopeTower.h"
#include "Camera/CameraManager.h"

ABuilding_TelescopeTower::ABuilding_TelescopeTower()
    : Super()

{
    uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_TelescopeTower"));
    RootComponent = uStaticMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Tower/Meshes/Tower/Telescopetower/SM_Building_BossMap.SM_Building_BossMap'"));
    if (MeshAsset.Succeeded())
        uStaticMesh->SetStaticMesh(MeshAsset.Object);
}

void ABuilding_TelescopeTower::BeginPlay()
{
    Super::BeginPlay();

    Init_Position = GetActorLocation();
}

void ABuilding_TelescopeTower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Operate_Building();
}

void ABuilding_TelescopeTower::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void ABuilding_TelescopeTower::Operate_Building()
{
    ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
    if (!MyCameraManager) return;

    if(2 <= MyCameraManager->Get_BCSecondCutScene_CameraIndex())
    {
        Operate_Timer += GetWorld()->GetDeltaSeconds();
        if (Operate_Timer <= 0.5f)
        {
            FVector CurrentPos = GetActorLocation();
            CurrentPos.Z -= GetWorld()->GetDeltaSeconds() * 1000.f;

            SetActorLocation(CurrentPos);
        }
        else if (Operate_Timer >= 3.5f)
        {
            FVector CurrentPos = GetActorLocation();
            CurrentPos.Z -= GetWorld()->GetDeltaSeconds() * 600.f;

            SetActorLocation(CurrentPos);
        }
    }
}


