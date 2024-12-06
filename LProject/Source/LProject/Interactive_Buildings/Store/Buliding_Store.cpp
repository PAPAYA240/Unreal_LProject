// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive_Buildings/Store/Buliding_Store.h"
#include "UI/InGame/BuildingEvent_Widget.h"
#include "UI/InGame/DialogEvent_Widget.h"
#include "Player/R1Player.h"

#include "Components/Button.h"

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h" 

ABuliding_Store::ABuliding_Store()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_Statue"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Art/Orasot_Bundle/Stylized_Landscape_5_Bioms/Global/StaticMeshes/SM_Rock_Stature1.SM_Rock_Stature1'"));
	if (MeshAsset.Succeeded())
	{
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Art/FXVarietyPack/Particles/P_ky_magicCircle1.P_ky_magicCircle1'"));
	if (ParticleAsset.Succeeded())
	{
		MagicParticle = ParticleAsset.Object;
	}
	else
		MSG_BOX(TEXT("no Particle Create"));
}

void ABuliding_Store::BeginPlay()
{
	Super::BeginPlay();

	Building_Type = EBUILDING_TYPE::HILL_BUILDING; 
	if (!Find_Widget())
		MSG_BOX(TEXT("Widget을 생성할 수 없습니다 : ABuliding_Store"));
}

void ABuliding_Store::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Opreate_Widget(); // Widget 작동
	CheckCondition();	// 기도를 없애는 조건 체크
}

bool ABuliding_Store::Find_Widget()
{
	/* 1.  Store Widget */
	if (BuildingEvent_Widget)
		BuildingEvent_Instance = Cast<UBuildingEvent_Widget>(CreateWidget(GetWorld(), BuildingEvent_Widget));

	if (BuildingEvent_Instance)
		BuildingEvent_Instance->AddToViewport();
	else
		return false;

	/* 2.  Dialog_Widget */
	if (Dialog_Widget)
		Dialog_Instance = Cast<UDialogEvent_Widget>(CreateWidget(GetWorld(), Dialog_Widget));

	if (Dialog_Instance)
	{
		Dialog_Instance->Set_BuildingType(Building_Type);
		Dialog_Instance->AddToViewport();
	}
	else
		return false;

	return true;
}

void ABuliding_Store::Opreate_Widget()
{
	/* 상호작용했을 때 */
	Show_Widget(BuildingEvent_Instance);

	// [기도하기]를 눌렀을 떄
	if (BuildingEvent_Instance->Get_Pray())
	{
		// 파티클 작동
		if (false == bOperateParticle)
		{
			bOperateParticle = true;
			UParticleSystemComponent* ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MagicParticle, this->GetActorLocation(), FRotator::ZeroRotator, true);

			FVector NewScaled(2.5f, 2.5f, 2.5f);
			ParticleSystem->SetWorldScale3D(NewScaled);
		}

		// [고마움] 대사 켜기
		Dialog_Instance->Set_ShowWidget(true);
		BuildingEvent_Instance->Set_ShowWidget(false);
	}
	// [떠나기]를 누를 시,
	else
		Exit_Widget(BuildingEvent_Instance);
}

void ABuliding_Store::CheckCondition()
{
	if (true == Exit_Widget(Dialog_Instance)) // 이야기가 끝이 났다면 기도 없애기
		BuildingEvent_Instance->Set_Pray(false);
}

void ABuliding_Store::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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