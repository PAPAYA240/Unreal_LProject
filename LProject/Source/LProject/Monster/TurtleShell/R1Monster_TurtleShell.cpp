

#include "Monster/TurtleShell/R1Monster_TurtleShell.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Item/R1Coin.h"

AR1Monster_TurtleShell::AR1Monster_TurtleShell()
	: Super()
{
}

void AR1Monster_TurtleShell::BeginPlay()
{
	Setting_Information();

	Super::BeginPlay();
}

void AR1Monster_TurtleShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_TurtleShell::Pattern_Function()
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
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)] = true;
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
	}
	return true;
}

void AR1Monster_TurtleShell::Impact_PlayerPattern()
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
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = true;
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

void AR1Monster_TurtleShell::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::HONGYEOP_TYPE;
	MonsterType = EMONSTER_TYPE::TURTLESHELL;
	MonsterName = FText::FromString(TEXT("거북 대령"));
	MonsterDocument = FText::FromString(TEXT("거북 대령은 전투에서 놀라운 속도를 자랑합니다."));

	Setting_MonsterAbility(2);
}
