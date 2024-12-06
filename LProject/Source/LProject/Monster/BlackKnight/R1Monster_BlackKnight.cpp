// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BlackKnight/R1Monster_BlackKnight.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

AR1Monster_BlackKnight::AR1Monster_BlackKnight()
	: Super()
{
	// Weapon Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> Weaponobj(TEXT("/Script/Engine.StaticMesh'/Game/Art/RPGMonsterWave2PA/Mesh/BlackKnight/AxeSM.AxeSM'"));

	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BLACKKNIGHT_WEAPON"));
	if (Weaponobj.Succeeded())
	{
		WeaponMeshComponent->SetStaticMesh(Weaponobj.Object);
		WeaponMeshComponent->SetupAttachment(GetRootComponent());
	}
}

void AR1Monster_BlackKnight::BeginPlay()
{
	Setting_Information();
	Super::BeginPlay();

	if(!Setting_Weapon())
		MSG_BOX(TEXT("AR1Monster_BlackKnight() : Weapon을 찾을 수 없습니다."));

}

void AR1Monster_BlackKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_BlackKnight::Pattern_Function()
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
		// true == ParticleStart(Hill_Particle, true, 2.f, this->GetActorLocation())
		if (Hill())
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

void AR1Monster_BlackKnight::Impact_PlayerPattern()
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

void AR1Monster_BlackKnight::Setting_Information()
{
	MonsterType = EMONSTER_TYPE::BLACKKNIGHT;
	MonsterName = FText::FromString(TEXT("블랙 나이트"));
	MonsterDocument = FText::FromString(TEXT("강력한 도끼를 휘두르며 적을 압도합니다."));

	Setting_MonsterAbility(5);
}

bool AR1Monster_BlackKnight::Setting_Weapon()
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
