

#include "Monster/Wolf/R1Monster_Wolf.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

AR1Monster_Wolf::AR1Monster_Wolf()
	: Super()
{
}

void AR1Monster_Wolf::BeginPlay()
{
	Setting_Information();
	Super::BeginPlay();
}

void AR1Monster_Wolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_Wolf::Pattern_Function()
{
	if (false == Super::Pattern_Function())
		return false;

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
	{
		bDead = Drop_Coin(this->GetActorLocation(), true, R1Player->GetActorLocation());
		return false;
	}

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)])
	{
		//true == ParticleStart(Hill_Particle, true, 2.f, this->GetActorLocation())
		if (Hill())
		{
			Ready_NextTurn(EMONSTER_PATTERN::HILL, EFIGHT_AUTHORITY::PLAYER);
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
	{
		if (true == HitAndDash(this, R1Player->Get_InitPos())) 
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER))/* 다음 턴으로 넘어가고*/
				Stop_HitAndDash();
		}
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
	{
		if (true == HitAndDash(this, R1Player->Get_InitPos()))
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::HIDDEN_ATTACK, EFIGHT_AUTHORITY::PLAYER))
				Stop_HitAndDash();
		}
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
	}
	return true;
}

void AR1Monster_Wolf::Impact_PlayerPattern()
{
	if (false == IsMonsterTurn())
		return;

	if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)] ||
		true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
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
		IsImpact = EIMPACT::IMPACT_END;
}

void AR1Monster_Wolf::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::DEYRON_TYPE;
	MonsterType = EMONSTER_TYPE::WOLF;
	MonsterName = FText::FromString(TEXT("고독한 늑대"));
	MonsterDocument = FText::FromString(TEXT("고독한 늑대는 그윽한 눈빛과 날카로운 감각을 지닌 존재입니다."));
	
	Setting_MonsterAbility(8);
}

