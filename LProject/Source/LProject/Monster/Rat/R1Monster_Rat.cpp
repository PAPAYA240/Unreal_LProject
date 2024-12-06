

#include "Monster/Rat/R1Monster_Rat.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

AR1Monster_Rat::AR1Monster_Rat()
	: Super()
{
	// Weapon Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> Weaponobj(TEXT("/Script/Engine.StaticMesh'/Game/Art/RPGMonsterWave2PA/Mesh/RatAssassin/RatDaggerSM.RatDaggerSM'"));

	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RAT_WEAPON"));
	if (Weaponobj.Succeeded())
	{
		WeaponMeshComponent->SetStaticMesh(Weaponobj.Object);
		WeaponMeshComponent->SetupAttachment(GetRootComponent());
	}
}

void AR1Monster_Rat::BeginPlay()
{
	Setting_Information();

	Super::BeginPlay();
	
	if (!Setting_Weapon())
		MSG_BOX(TEXT("AR1Monster_Rat() : Weapon을 찾을 수 없습니다."));
}

void AR1Monster_Rat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();


}

bool AR1Monster_Rat::Pattern_Function()
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
		if (true == HitAndDash(this, R1Player->Get_InitPos())) /* 대시가 끝났다면*/
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER))/* 다음 턴으로 넘어가고*/
				Stop_HitAndDash(); /* 대시를 다시 할 수 있도록 만들 거임*/
		}
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
	{
		if (true == HitAndDash(this, R1Player->Get_InitPos(), 1.f))
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

void AR1Monster_Rat::Impact_PlayerPattern()
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

void AR1Monster_Rat::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::HONGYEOP_TYPE;
	MonsterType = EMONSTER_TYPE::RAT;
	MonsterName = FText::FromString(TEXT("지하 생쥐"));
	MonsterDocument = FText::FromString(TEXT("강인한 갈퀴를 이용하여 힘을 약화시킵니다."));
	
	Setting_MonsterAbility(5);
}

bool AR1Monster_Rat::Setting_Weapon()
{
	FName Soket_Name(TEXT("Weapon_Socket"));
	if (GetMesh() && GetMesh()->DoesSocketExist(Soket_Name))
	{
		WeaponMeshComponent->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			Soket_Name
		);
	}
	else
		return false;
	return true;
}
