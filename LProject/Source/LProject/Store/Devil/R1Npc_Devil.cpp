// Fill out your copyright notice in the Description page of Project Settings.


#include "Store/Devil/R1Npc_Devil.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "UI/InGame/BuildingEvent_Widget.h"

AR1Npc_Devil::AR1Npc_Devil()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPC_Devil"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Art/hornman_skull.hornman_skull'"));
	if (MeshAsset.Succeeded())
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
}

void AR1Npc_Devil::BeginPlay()
{
	Super::BeginPlay();

	if (!Setting_Widget())
		MSG_BOX(TEXT("AR1Npc_Devil() : Widget이 세팅되지 않았습니다."));
}

void AR1Npc_Devil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Operate_Widget(); // Widget 작동
	Floundering(); // 둥실 떠다니기

	Exit_Widget(Event_Instance); // Widget 접었는 지 확인
}

void AR1Npc_Devil::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (AR1Character* Player = Cast<AR1Character>(OtherActor))
		{
			bStart = true;
		}
	}
}

bool AR1Npc_Devil::Setting_Widget()
{
	if (Event_Widget)
	{
		Event_Instance = Cast<UBuildingEvent_Widget>(CreateWidget(GetWorld(), Event_Widget));
		Event_Instance->AddToViewport();
		Event_Instance->Setting_WidgetText(EBUILDING_TYPE::DEVIL_EVENT);
	}
	else
		return false;

	Origin_Pos = this->GetActorLocation();

	return true;
}

void AR1Npc_Devil::Floundering()
{
	FVector CurrentPos = this->GetActorLocation();
	FVector TargetPos = Origin_Pos;
	TargetPos.Z += 150.f;

	if (10.f >= FVector::Dist(Origin_Pos, this->GetActorLocation()))
		bFloundering = true;
	else if (10.f >= FVector::Dist(TargetPos, this->GetActorLocation()))
		bFloundering = false;

	// 업
	if (true == bFloundering)
	{
		FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 1.f);
		this->SetActorLocation(NewPos);
	}
	// 다운
	else if (false == bFloundering)
	{
		FVector NewPos = FMath::VInterpTo(CurrentPos, Origin_Pos, GetWorld()->GetDeltaSeconds(), 1.f);
		this->SetActorLocation(NewPos);
	}
}

void AR1Npc_Devil::Operate_Widget()
{
	if (Event_Instance->Get_Pray())
	{
		// 계약 승낙 시
		if(ESlateVisibility::Hidden != Event_Instance->Get_ShowWidget())
			Event_Instance->Set_ShowWidget(false);

		R1Player->DecreaseHP(R1Player->CurrentHp, R1Player->Hp, GetWorld()->GetDeltaSeconds());

		if (R1Player->CurrentHp == R1Player->Hp)
		{
			Event_Instance->Set_Exit(true);
			Event_Instance->Set_Pray(false);
		}
	}
	else
	{
		Show_Widget(Event_Instance);
	}
}

