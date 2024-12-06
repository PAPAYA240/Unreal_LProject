

#include "Interactive_Buildings/Object/Building_Tent.h"
#include "UI/InGame/ChallengeDialog_Widet.h"
#include "UI/InGame/TreasureCard_Widget.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Camera/CameraManager.h"

ABuilding_Event::ABuilding_Event()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_EventBuilding"));
	uStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	uStaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	uStaticMesh->SetGenerateOverlapEvents(true);
}

void ABuilding_Event::BeginPlay()
{
	Building_Type = EBUILDING_TYPE::MONK_STATUE;
	Super::BeginPlay();

	if (!Find_Widget())
		MSG_BOX(TEXT("Widget을 생성할 수 없습니다 : ABuilding_Statue"));
}

void ABuilding_Event::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ChallengeDialog_Instance)
	{
		ChallengeDialog_Type();
	}
	else if (RandomCard_Instance)
	{
		RandomCard_Type();
	}
}

void ABuilding_Event::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (AR1Character* Player = Cast<AR1Character>(OtherActor))
		{
			bStart = true;

			if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
				R1Player->Get_CurrentPlayerState()->TargetObject = this;
		}
	}
}

void ABuilding_Event::ChallengeDialog_Type()
{
	if (true == ChallengeDialog_Instance->Get_Exit())
	{
		Stop_Time += GetWorld()->GetDeltaSeconds();
		if (Stop_Time >= 3.f) // 3초의 시간 텀을 두고 widget을 닫는다.
			Exit_Widget(ChallengeDialog_Instance);
	}
	Show_Widget(ChallengeDialog_Instance);
}

void ABuilding_Event::RandomCard_Type()
{
	if (true == RandomCard_Instance->Get_Exit())
	{
		Stop_Time += GetWorld()->GetDeltaSeconds();
		if (Stop_Time >= 3.f) // 3초의 시간 텀을 두고 widget을 닫는다.
			Exit_Widget(RandomCard_Instance);
	}
	Show_Widget(RandomCard_Instance);
}

bool ABuilding_Event::Find_Widget()
{
	if (ChallengeDialog_Widget)
	{
		ChallengeDialog_Instance = Cast<UChallengeDialog_Widet>(CreateWidget(GetWorld(), ChallengeDialog_Widget));
		if (ChallengeDialog_Instance)
		{
			ChallengeDialog_Instance->AddToViewport();
			ChallengeDialog_Instance->Set_BuildingType(Building_Type);
		}
		else
			return false;
	}

	else if (RandomCard_Widget)
	{
		RandomCard_Instance = Cast<UTreasureCard_Widget>(CreateWidget(GetWorld(), RandomCard_Widget));
		if (RandomCard_Instance)
		{
			RandomCard_Instance->AddToViewport();
		}
		else
			return false;
	}
	else
		return false;

	return true;
}