// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive_Buildings/RestArea/Building_RestArea.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Camera/CameraManager.h"

#include "UI/InGame/Store/RestArea_Widget.h"

ABuilding_RestArea::ABuilding_RestArea()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_RestArea"));
	uStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	uStaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	uStaticMesh->SetGenerateOverlapEvents(true);
}

void ABuilding_RestArea::BeginPlay()
{
	Super::BeginPlay();

	if (!Find_Widget())
		MSG_BOX(TEXT("Widget을 생성할 수 없습니다 : ABuilding_RestArea"));
}

bool ABuilding_RestArea::Find_Widget()
{
	if (RestArea_Widget)
		RestArea_Instance = Cast<URestArea_Widget>(CreateWidget(GetWorld(), RestArea_Widget));

	if (RestArea_Instance)
		RestArea_Instance->AddToViewport();
	else
		return false;

	return true;
}

void ABuilding_RestArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == RestArea_Instance->Get_Exit())
		Exit_Widget(RestArea_Instance);
	Show_Widget(RestArea_Instance);
}

void ABuilding_RestArea::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
