

#include "Interactive_Buildings/ReconTower/R1ReconTower.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Interactive_Buildings/Telescope/Building_Telescope.h"
#include "UI/InGame/Mission_Widget.h"

#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/R1UserWidget.h"
#include "Components/ProgressBar.h"

#include "Camera/CameraManager.h"
#include "System/R1GameInstance.h"

#include "Environment/Environment_Cloud.h"

#define CAMERA_DIST		800.f // this와 Cam 간의 거리

AR1ReconTower::AR1ReconTower()
	: Super()
{
	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	RootComponent = MyMesh; 

	// WidgetComponent 생성
	HpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RECON_3DWIDGET"));
	HpWidget->SetupAttachment(MyMesh);
	HpWidget->SetWidgetSpace(EWidgetSpace::World);
	HpWidget->SetTwoSided(true);

	// 위젯 블루프린트 로드
	static ConstructorHelpers::FClassFinder<UUserWidget> HPWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/InGame_Default_UI/ReconTower_HP.ReconTower_HP_C'"));
	if (HPWidget.Succeeded())
	{
		HpWidget->SetWidgetClass(HPWidget.Class);
		HpWidget->SetDrawSize(FVector2D(1000, 1000));
	}
}

void AR1ReconTower::BeginPlay()
{
	Super::BeginPlay();

	if (!Setting_BossTower())
		MSG_BOX(TEXT("AR1ReconTower : Setting_BossTower()가 세팅되지 않았습니다."));

	if(!Settinng_Widget())
		MSG_BOX(TEXT("AR1ReconTower : Settinng_Widget()가 세팅되지 않았습니다."));

	if (!Setting_Scaled())
		MSG_BOX(TEXT("AR1ReconTower : Setting_Scaled()가 세팅되지 않았습니다."));
}

void AR1ReconTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Operate_Default();
	Operate_Workable();

	if(true == bWorkable)
	{
		// 셀레시아 정찰탑에 맞닿았다면, 
		if (EREGION_TYPE::SELISIA_TYPE == Recon_Type)
		{
			if (MissionWidget)
				MissionWidget->Set_CurrentMission(EMISSION_TYPE::VISIT_MARRYFATHER);
		}

	}
}

void AR1ReconTower::Find_ReconTower()
{
	// 플레이어와의 거리 비교
	if (false == bFindReconTower)
	{
		FVector PlayerPos = R1Player->GetActorLocation();
		FVector TowerPos = this->GetActorLocation();
		double PlayertoDist = FVector::Dist(PlayerPos, TowerPos);

		if (800.f >= PlayertoDist)
			bFindReconTower = true;

		return;
	}
	else if (true == bFindReconTower)
	{
		if(false == bJudgCloud)
		{
			// 주변 구름 없애기
			TArray<AEnvironment_Cloud*> CloudArray;
			UObject* WorldContextObject = GetWorld();
			CloudArray = AR1Player::FindActorsByClass<AEnvironment_Cloud>(WorldContextObject);
			for (auto iter : CloudArray)
			{
				iter->AroundCloud_Destroy(this->GetActorLocation());
			}
		}
	}

	// ReconTower에 포커스를 맞춘다.
	Focus_ReconTower(); 
}

void AR1ReconTower::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (AR1Character* Player = Cast<AR1Character>(OtherActor))
		{
			// 1. 작동 가능하다면 내린다.
			if (true == bWorkable)
				bOperate = true;
		}
	}
}

void AR1ReconTower::ReconTower_Rotation()
{
	FRotator CurrentRotation = this->GetActorRotation();
	FRotator NewRotation = CurrentRotation + FRotator(0, GetWorld()->GetDeltaSeconds() * RotationSpeed, 0);
	this->SetActorRotation(NewRotation);
}

void AR1ReconTower::Opreate_OriginScale()
{
	if (false == bFindReconTower)
		return;

	/* 가까이 다가왔을 때 사이즈를 키우도록 함*/
	if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
	{
		FVector CurrentCameraPos = MyCameraManager->GetCameraLocation();
		FVector CurrentReconPos = this->GetActorLocation();

		if (CAMERA_DIST <= FVector::DistXY(CurrentCameraPos, CurrentReconPos))
			return;

		// 1. 크기 키우기
		FVector CurrentScaled = this->GetActorScale3D();
		FVector TargetScaled = OriginScale;
		FVector NewScaled = FMath::VInterpTo(CurrentScaled, TargetScaled, GetWorld()->GetDeltaSeconds(), 5.f);
		double ScaledDist = FVector::Dist(NewScaled, TargetScaled);

		if (ScaledDist < KINDA_SMALL_NUMBER)
			return;
		else
			this->SetActorScale3D(NewScaled);
	}
}

void AR1ReconTower::Focus_ReconTower()
{
	if(true == bTargetReconTower && nullptr == R1Player->Get_CurrentPlayerState()->TargetObject)
	{
		bStart = false;
		return;
	}

	if (false == bTargetReconTower)
	{
		bTargetReconTower = true;
		R1Player->Get_CurrentPlayerState()->TargetObject = this;
		bStart = true;
	}
}

void AR1ReconTower::LookCamera_Widget()
{
	if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
	{
		FVector Current_CamPos = MyCameraManager->GetCameraLocation();
		FVector Current_MeshPos = HpWidget->GetComponentLocation();

		FVector DirectionToCamera = (Current_CamPos - Current_MeshPos).GetSafeNormal();
		FRotator RotationToCam = DirectionToCamera.Rotation();
		HpWidget->SetWorldRotation(RotationToCam);
	}
}

void AR1ReconTower::PercentChange()
{
	if (UProgressBar* RedHPBar_Progress = Cast<UProgressBar>(ReconHp_Instance->GetWidgetFromName(TEXT("RedHPBar"))))
	{
		RedHPBar_Progress->SetPercent(RedHPBar_Progress->GetPercent() + 0.1f);

		if (1.f <= RedHPBar_Progress->GetPercent())
			bWorkable = true;
	}
}

bool AR1ReconTower::Setting_Scaled()
{
	OriginScale = this->GetActorScale3D();
	FVector Scaled = FVector(0.2f, 0.2f, 0.2f);
	this->SetActorScale3D(Scaled);
	return true;
}

bool AR1ReconTower::Settinng_Widget()
{
	TArray<UUserWidget*> FoundWidgets;
	UObject* WorldContextObject = GetWorld();
	FoundWidgets = AR1Player::FindWidgetsByClass<UMission_Widget>(WorldContextObject);

	if (FoundWidgets.Num() > 0)
	{
		UMission_Widget* FoundWidget = Cast<UMission_Widget>(FoundWidgets[0]);
		if (FoundWidget)
			MissionWidget = FoundWidget;
	}

	if (HpWidget)
	{
		HpWidget->InitWidget();
		ReconHp_Instance = Cast<UR1UserWidget>(HpWidget->GetUserWidgetObject());
	}
	return true;
}

bool AR1ReconTower::Setting_BossTower()
{
	UObject* WorldContextObject = GetWorld();
	TArray<ABuilding_Telescope*> TelescopeArray = AR1Player::FindActorsByClass<ABuilding_Telescope>(WorldContextObject);

	if (0 <= TelescopeArray.Num())
		BossTower = TelescopeArray[0];

	if (nullptr == BossTower)
		return false;
	else
		return true;
}

void AR1ReconTower::Operate_Default()
{
	Find_ReconTower();
	ReconTower_Rotation(); // 실시간으로 돌아가는 Rotator
	Opreate_OriginScale(); // 발견 시에 Scaled 키우기
	LookCamera_Widget(); // Widget : 카메라를 바라본다.
}

void AR1ReconTower::Operate_Workable()
{
	if (false == bWorkable || false == bOperate)
		return;

	if (nullptr == BossTower)
		return;

	// BT 아래로 움직이기
	WorkableTimer += GetWorld()->GetDeltaSeconds();
	if (1.5f <= WorkableTimer)
		BossTower->Set_Operate(true);
}
