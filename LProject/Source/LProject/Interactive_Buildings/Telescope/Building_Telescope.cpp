// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive_Buildings/Telescope/Building_Telescope.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Camera/CameraManager.h"

#define CHANGE_TIMER 1.7f

ABuilding_Telescope::ABuilding_Telescope()
	: Super()
{
    uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_Telescope"));
    RootComponent = uStaticMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Tower/Meshes/Tower/Telescope/SM_SM_TotelScope.SM_SM_TotelScope'"));
    if (MeshAsset.Succeeded())
        uStaticMesh->SetStaticMesh(MeshAsset.Object);
}

void ABuilding_Telescope::BeginPlay()
{
	Super::BeginPlay();

    Building_Type = EBUILDING_TYPE::TELESCOPE_BUILDING;
}

void ABuilding_Telescope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpenBossMap();

	if(true == bOperate)
		Operate_BossMap();
}

void ABuilding_Telescope::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (AR1Character* Player = Cast<AR1Character>(OtherActor))
		{
			bStart = true;
		}
	}
}

void ABuilding_Telescope::OpenBossMap()
{
	if (bStart)
	{
		ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
		if (true == MyCameraManager->EnterBossMap_Camera())
		{
			ChangeLevel_Timer += GetWorld()->GetDeltaSeconds();

			if (ChangeLevel_Timer >= CHANGE_TIMER)
			{
				// Ui로 화면을 검정색으로 만들거나...화면 씬 만들기
				R1Player->Change_Level(ESCENE::BOSS_SCENE);
			}
		}
	}
}

void ABuilding_Telescope::Operate_BossMap()
{
	if (false == bInitPos)
	{
		BT_InitPos = this->GetActorLocation();
		bInitPos = true;

		ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
		MyCameraManager->Set_OperateTelescope(true);
	}

	FVector BT_CurrentPos = this->GetActorLocation();
	FVector BT_TargetPos = BT_InitPos;
	BT_TargetPos.Z -= 300.f;
	FVector New_BTPos = FMath::VInterpTo(BT_CurrentPos, BT_TargetPos, GetWorld()->GetDeltaSeconds(), 5.f);
	
	if (10.f >= FVector::Dist(New_BTPos, BT_TargetPos))
	{
		bOperate = false;
		bInitPos = false;

		ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
		MyCameraManager->Set_OperateTelescope(false);
	}
}
