// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive_Buildings/PlayerSpawner/Building_PlayerSpawner.h"
#include "Components/StaticMeshComponent.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "UI/InGame/DialogEvent_Widget.h"

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h"


ABuilding_PlayerSpawner::ABuilding_PlayerSpawner()
	: Super()
{
	// Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Art/FXVarietyPack/Particles/P_ky_thunderStorm.P_ky_thunderStorm'"));
	if (ParticleAsset.Succeeded())
		SpawnerParticle = ParticleAsset.Object;
}

void ABuilding_PlayerSpawner::BeginPlay()
{
	Super::BeginPlay();

	Building_Type = EBUILDING_TYPE::PLAYER_START_SPAWNER_BUILIDING;

	if (!PlayerPosition_Setting())
		MSG_BOX(TEXT("ABuilding_PlayerSpawner() : Player Position 이 제대로 세팅되지 않았습니다."));
	
	if (!Find_Widget())
		MSG_BOX(TEXT("Widget을 생성할 수 없습니다 : ABuilding_PlayerSpawner"));
}

void ABuilding_PlayerSpawner::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
	
	Operate_BossMap();
	Opreate_InGame();
}

bool ABuilding_PlayerSpawner::Find_Widget()
{
	/* 1. Monster HP Widget */
	if (Dialog_Widget)
		Dialog_Instance = Cast<UDialogEvent_Widget>(CreateWidget(GetWorld(), Dialog_Widget));

	if (Dialog_Instance)
	{
		Dialog_Instance->Set_BuildingType(Building_Type);
		Dialog_Instance->AddToViewport(2);
	}
	else
		return false;
	return true;
}

void ABuilding_PlayerSpawner::Exit_SpawnerWidget()
{
	if (nullptr == Dialog_Instance)
		return;

	if (true == bDialogEnd)
		return;

	if (true == Dialog_Instance->Get_Exit())
	{
		Return_InGame();
		CurrentPlayerState->TargetObject = this; // Mission Bar에서 nullptr 처리 해줄 것임

		Dialog_Instance->Set_ShowWidget(false);
		Dialog_Instance->Set_Exit(false); // 플레이어 카메라를 되돌린다.

		bDialogEnd = true;
		R1Player->Set_NextTurnStart();
	}
}

void ABuilding_PlayerSpawner::Stop_SpawnParticle()
{
	if (ParticleSystem)
	{
		if (ParticleSystem->IsActive())
		{
			Particle_Timer += GetWorld()->GetDeltaSeconds();
			if (Particle_Timer >= 1.f)
			{
				ParticleSystem->Deactivate();
				ParticleSystem = nullptr;
			}
		}
	}
}

void ABuilding_PlayerSpawner::Operate_SpawnParticle()
{
	if (false == bParticleStart)
	{
		bParticleStart = true;

		FVector SpawnerPos = this->GetActorLocation();
		ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnerParticle, SpawnerPos, FRotator::ZeroRotator, true);
	}
}

void ABuilding_PlayerSpawner::Operate_BossMap()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (nullptr == GameMode->CurrentPlayer || nullptr == GameMode || ESCENE::BOSS_SCENE != GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene)
		return;

	if (false == bBossMap_Setting)
	{
		if (ESCENE::BOSS_SCENE == GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene)
		{
			FVector SpawnerLocation = this->GetActorLocation();
			FRotator SpawnerRotation = this->GetActorRotation();
			SpawnerLocation.Z += 200.f;
			SpawnerLocation.Z = 410.250151f;
			if (ESPAWN_TYPE::BLUE_SPAWN == Spawn_Type)
			{
				GameMode->PlayerArray[0]->SetActorLocationAndRotation(SpawnerLocation, SpawnerRotation);
				bBossMap_Setting = true;
			}

			else if (ESPAWN_TYPE::RED_SPAWN == Spawn_Type)
			{
				GameMode->PlayerArray[1]->SetActorLocationAndRotation(SpawnerLocation, SpawnerRotation);
				bBossMap_Setting = true;
			}

			else if (ESPAWN_TYPE::GREEN_SPAWN == Spawn_Type)
			{
				GameMode->PlayerArray[2]->SetActorLocationAndRotation(SpawnerLocation, SpawnerRotation);
				bBossMap_Setting = true;
			}
		}
	}
	else if (bBossMap_Setting)
	{
		this->Destroy();
	}
	

}

void ABuilding_PlayerSpawner::Opreate_InGame()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (nullptr == GameMode->CurrentPlayer || nullptr == GameMode || ESCENE::INGAME != GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene)
		return;

	R1Player = GameMode->CurrentPlayer;
	if (ESCENE::INGAME == R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
	{
		if (ESPAWN_TYPE::BLUE_SPAWN == Spawn_Type)
		{
			Exit_SpawnerWidget();
			// 게임의 첫 시작이 아니라면 bDialogEnd는 true로 해준다.
			if (ESCENE::FIGHT_SCENE == R1Player->Get_CurrentPlayerState()->Player_PrevScene)
				bDialogEnd = true;

			// 게임 첫 시작의 Dialog
			if (false == bDialogEnd)
			{
				R1Player->Get_CurrentPlayerState()->TargetObject = this;
				Stop_SpawnParticle();

				if (R1Player->Get_GamePlayStart())
				{
					if (Dialog_Instance)
						Dialog_Instance->Set_ShowWidget(true);
				}
			}
		}
	}
}

void ABuilding_PlayerSpawner::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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

bool ABuilding_PlayerSpawner::PlayerPosition_Setting()
{
	// 1. Spawner에 Player 위치 지정
	if (AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		for (int i = 0; i < 3; i++)
		{
			if (nullptr == GameMode->PlayerArray[i])
			{
				MSG_BOX(TEXT("GameMode 플레이어 어레이가 없습니다."));
				return false;
			}
		}

		/* 1. 처음 InGame에 들어갔을 때는 Spawner에서 생성 */
		if (ESCENE::INGAME == GameMode->PlayerArray[0]->Get_CurrentPlayerState()->Player_CurrentScene &&
			ESCENE::LOBBY == GameMode->PlayerArray[0]->Get_CurrentPlayerState()->Player_PrevScene)
		{
			FVector SpawnerLocation = this->GetActorLocation();
			SpawnerLocation.Z += 200.f;

			if (ESPAWN_TYPE::BLUE_SPAWN == Spawn_Type)
				GameMode->PlayerArray[0]->SetActorLocation(SpawnerLocation);

			else if (ESPAWN_TYPE::RED_SPAWN == Spawn_Type)
				GameMode->PlayerArray[1]->SetActorLocation(SpawnerLocation);

			else if (ESPAWN_TYPE::GREEN_SPAWN == Spawn_Type)
				GameMode->PlayerArray[2]->SetActorLocation(SpawnerLocation);
		}
		/* 2. 그렇지 않다면 마지막에 존재했던 Location에서 생성*/
		else
		{
			for (int i = 0; i < 3; i++)
				GameMode->PlayerArray[i]->SetActorLocation(GameMode->PlayerArray[i]->Get_CurrentPlayerState()->LastPlayerPosition);
		}
	}
	return true;
}