

#include "Monster/Specter/R1Specter_Monster.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

AR1Specter_Monster::AR1Specter_Monster()
	: Super()
{
	// Weapon Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> Weaponobj(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_RockSpikes_01c.Par_LoPo_RockSpikes_01c'"));

	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SPECTER_WEAPON"));
	if (Weaponobj.Succeeded())
	{
		WeaponMeshComponent->SetStaticMesh(Weaponobj.Object);
		WeaponMeshComponent->SetupAttachment(GetRootComponent());
	}
}

void AR1Specter_Monster::BeginPlay()
{
	Setting_Information();

	Super::BeginPlay();

	if(!Setting_Weapon())
		MSG_BOX(TEXT("R1Specter_Monster() : Failed Setting Weapon"))
}

void AR1Specter_Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Specter_Monster::Pattern_Function()
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
		FVector TargetPlayerPos = R1Player->Get_InitPos();
		TargetPlayerPos.Y -= 250.f;
		if (true == HitAndDash(this, TargetPlayerPos))
		{
			if (true == Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER))
				Stop_HitAndDash(); 
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
	{
		// 타이밍에 맞춰 파티클 생성
		if(true == bAttackStart)
		{
			FVector TargetPlayerPos = R1Player->Get_InitPos();
			if (true == ParticleStart(Hidden_Particle, true, 2.f, TargetPlayerPos))
			{
				if(Ready_NextTurn(EMONSTER_PATTERN::HIDDEN_ATTACK, EFIGHT_AUTHORITY::PLAYER, 1.5f))
					bAttackStart = false;
			}
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
	}
	return true;
}

void AR1Specter_Monster::Impact_PlayerPattern()
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

void AR1Specter_Monster::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::SELISIA_TYPE;
	MonsterType = EMONSTER_TYPE::SPECTER;
	MonsterName = FText::FromString(TEXT("심판 유령"));
	MonsterDocument = FText::FromString(TEXT("강력한 마법으로 적을 짓누릅니다."));

	Setting_MonsterAbility(3);
}

bool AR1Specter_Monster::Setting_Weapon()
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
