

#include "Monster/Swarm/R1Monster_Swarm.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Item/R1Coin.h"

AR1Monster_Swarm::AR1Monster_Swarm()
	: Super()
{
}

void AR1Monster_Swarm::BeginPlay()
{
	Setting_Information();
	
	Super::BeginPlay();
}

void AR1Monster_Swarm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_Swarm::Pattern_Function()
{
	if (false == Super::Pattern_Function())
		return false;

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
	{
		bDead = Drop_Coin(this->GetActorLocation(), true, R1Player->GetActorLocation());
		return false;
	}

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)] ||
		true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)])
	{
		Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)] = false;
		Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)] = false;
		Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = true;
	}

	 if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
	{
		if (true == HitAndDash(this, R1Player->Get_InitPos())) /* 대시가 끝났다면*/
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER))/* 다음 턴으로 넘어가고*/
				Stop_HitAndDash(); /* 대시를 다시 할 수 있도록 만들 거임*/
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
	}
	 return true;
}

void AR1Monster_Swarm::Impact_PlayerPattern()
{
	if (false == IsMonsterTurn())
		return;

	if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)] ||
		true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
	{
		if (EIMPACT::IMPACT_ATTACK != IsImpact)
		{
			IsImpact = EIMPACT::IMPACT_ATTACK;

			if (true == CalculateAbility((ABILITY_TYPE::DEFENCE)))
			{
				MSG_BOX(TEXT("MONSTER 회피 성공"));
			}
			else
			{
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = true;
			}
		}
	}
	else
		IsImpact = EIMPACT::IMPACT_END;
}

void AR1Monster_Swarm::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::HONGYEOP_TYPE;
	MonsterType = EMONSTER_TYPE::SWARM;
	MonsterName = FText::FromString(TEXT("탐지 해양 생물"));
	MonsterDocument = FText::FromString(TEXT("영역을 위협하는 자에게는 결코 관대하지 않은 구역의 지킴이입니다."));

	Setting_MonsterAbility(3);

}
