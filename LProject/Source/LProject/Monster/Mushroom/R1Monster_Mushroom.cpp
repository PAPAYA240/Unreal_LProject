#include "Monster/Mushroom/R1Monster_Mushroom.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

AR1Monster_Mushroom::AR1Monster_Mushroom() 
	: Super()
{
}

void AR1Monster_Mushroom::BeginPlay()
{
	Setting_Information();
	
	Super::BeginPlay();
}

void AR1Monster_Mushroom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_Mushroom::Pattern_Function()
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
		if (Hill()/*(true == ParticleStart(Hill_Particle, true, 2.f, this->GetActorLocation())*/)
		{
			Ready_NextTurn(EMONSTER_PATTERN::HILL, EFIGHT_AUTHORITY::PLAYER);
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
	{
		if (true == HitAndDash(this, R1Player->Get_InitPos()))
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER))
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
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::READY_HIT)])
	{
		if (true == R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
		{
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::READY_HIT)] = false;
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = true;
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
	}
	return true;
}


void AR1Monster_Mushroom::Impact_PlayerPattern()
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
				/* READY_HIT : 타이밍 맞춰서 애니메이션을 동작시킬 것임*/
				if(CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
				{
					Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::READY_HIT)] = true;
				}
				else 
					Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = true;
			}
		}
	}
	else
		IsImpact = EIMPACT::IMPACT_END;
}

void AR1Monster_Mushroom::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::DEYRON_TYPE;
	MonsterType = EMONSTER_TYPE::MUSHROOM;
	MonsterName = FText::FromString(TEXT("독버섯"));
	MonsterDocument = FText::FromString(TEXT("화려한 색상과 매력적인 모습으로 적을 유혹합니다."));
	
	Setting_MonsterAbility(4);
}


