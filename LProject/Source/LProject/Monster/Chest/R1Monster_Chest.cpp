

#include "Monster/Chest/R1Monster_Chest.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Item/R1Coin.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraManager.h"
AR1Monster_Chest::AR1Monster_Chest()
	: Super()
{	
}

void AR1Monster_Chest::BeginPlay()
{
	Setting_Information();

	Super::BeginPlay();
}

void AR1Monster_Chest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_Chest::Pattern_Function()
{
	if (false == Super::Pattern_Function())
		return false;

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
	{
		bDead = Drop_Coin(this->GetActorLocation(), true);
		return false;
	}

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)])
	{
		if (true == ParticleStart(Hill_Particle, true, 2.f, this->GetActorLocation()))
		{
			Ready_NextTurn(EMONSTER_PATTERN::HILL, EFIGHT_AUTHORITY::PLAYER);
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
	{
		if (true == HitAndDash(this, R1Player->Get_InitPos()))
		{
			if(true == Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER))
				Stop_HitAndDash();
		}
		bHitAndDashStart = false;
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
	{
		// Player Rotation
		FVector AtPlayerDistance = R1Player->GetActorLocation() - this->GetActorLocation();
		FRotator AtPlayerRot = AtPlayerDistance.Rotation();
		this->SetActorRotation(AtPlayerRot);

		FVector TargetPos = R1Player->Get_InitPos();
		TargetPos.X -= 70.f;

		if (true == HitAndDash(this, TargetPos))
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::HIDDEN_ATTACK, EFIGHT_AUTHORITY::PLAYER))
				Stop_HitAndDash();
		}
		bHitAndDashStart = false;
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		Start_Emissive(EMONSTER_PATTERN::HIT);

		if (false == bHitCameraZoom)
		{
			ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
			if (MyCameraManager)
			{
				if (MyCameraManager->PitchRotation_Camera(0.1f, 300.f))
				{
					++RotationCnt;
					if(2 == RotationCnt)
						bHitCameraZoom = true;
				}
			}
		}

		if (true == Hit())
		{
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
			bHitCameraZoom = false;
		}
	}
	return true;
}

void AR1Monster_Chest::Impact_PlayerPattern()
{
	if (false == IsMonsterTurn())
		return;

	if ((true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)] ||
		true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)]) 
		&& R1Player->Get_TimingAttack()) // 타이밍 맞춰서 때림
	{
		if (EIMPACT::IMPACT_ATTACK != IsImpact)
		{
			IsImpact = EIMPACT::IMPACT_ATTACK;

			/*if (true == CalculateAbility((ABILITY_TYPE::DEFENCE)))
			{
				MSG_BOX(TEXT("MONSTER 회피 성공"));
			}
			else*/
			{
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = true;
			}
		}
	}
	else
	{
		if(false == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
		{
			IsImpact = EIMPACT::IMPACT_END;
		}
	}
}

void AR1Monster_Chest::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::SELISIA_TYPE;
	MonsterType = EMONSTER_TYPE::CHEST;
	MonsterName = FText::FromString(TEXT("보물상자 도둑"));
	MonsterDocument = FText::FromString(TEXT("보물 상자로 분장해 공격하는 교활한 몬스터 입니다."));

	Setting_MonsterAbility(1);
}

