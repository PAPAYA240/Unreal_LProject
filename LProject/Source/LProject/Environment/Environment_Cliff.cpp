// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/Environment_Cliff.h"
#include "Player/R1Player.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

AEnvironment_Cliff::AEnvironment_Cliff()
{
	PrimaryActorTick.bCanEverTick = true;

	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Environment_Cliff"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Art/Orasot_Bundle/Stylized_Landscape_5_Bioms/Biom_Bamboo/StaticMeshes/SM_Make_UpCliff.SM_Make_UpCliff'"));
	if (MeshAsset.Succeeded())
	{
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
	}
}

void AEnvironment_Cliff::BeginPlay()
{
	Super::BeginPlay();

	if (!Find_Player())
		MSG_BOX(TEXT("AEnvironment_Cliff() : 플레이어를 찾지 못했습니다"));

	Init_Pos = this->GetActorLocation();
}

void AEnvironment_Cliff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Find_Player())
		MSG_BOX(TEXT("AEnvironment_Cliff() : 플레이어를 찾지 못했습니다"));

	if (true == bMoving_cliff)
	{
		if (false == bPlayerPos)
		{
			bPlayerPos = true;
			PlayerTarget_Pos = R1Player->GetActorLocation();
		}
		else if (true == bPlayerPos)
		{
			FVector TargetPos = Init_Pos;
			TargetPos.Z = PlayerTarget_Pos.Z - 200.f;

			FVector NewPos = FMath::VInterpTo(this->GetActorLocation(), TargetPos, GetWorld()->GetDeltaSeconds(), 2.f);

			this->SetActorLocation(NewPos);
		}
	}
	else if (false == bMoving_cliff)
	{
		bPlayerPos = false;
		this->SetActorLocation(Init_Pos);
	}
}

bool AEnvironment_Cliff::Find_Player()
{
	if (R1Player)
		return true;

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
		R1Player = GameMode->CurrentPlayer;

	return (nullptr != R1Player) ? true : false;
}

