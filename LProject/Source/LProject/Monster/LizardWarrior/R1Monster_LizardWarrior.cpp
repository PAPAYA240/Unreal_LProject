

#include "Monster/LizardWarrior/R1Monster_LizardWarrior.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

AR1Monster_LizardWarrior::AR1Monster_LizardWarrior()
	: Super()
{
	// Weapon Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> LWeaponobj(TEXT("/Script/Engine.StaticMesh'/Game/Art/RPGMonsterWave2PA/Mesh/LizardWarrior/LizardShieldSM.LizardShieldSM'"));

	Left_WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RAT_DEFENCE_WEAPON"));
	if (LWeaponobj.Succeeded())
	{
		Left_WeaponMeshComponent->SetStaticMesh(LWeaponobj.Object);
		Left_WeaponMeshComponent->SetupAttachment(GetRootComponent());
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> RWeaponobj(TEXT("/Script/Engine.StaticMesh'/Game/Art/RPGMonsterWave2PA/Mesh/LizardWarrior/LizardSwordSM.LizardSwordSM'"));

	Right_WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RAT_KNIFE_WEAPON"));
	if (RWeaponobj.Succeeded())
	{
		Right_WeaponMeshComponent->SetStaticMesh(RWeaponobj.Object);
		Right_WeaponMeshComponent->SetupAttachment(GetRootComponent());
	}
}

void AR1Monster_LizardWarrior::BeginPlay()
{
	Setting_Information();
	Super::BeginPlay();

	if (!Setting_Weapon())
		MSG_BOX(TEXT("AR1Monster_LizardWarrior() : Weapon을 찾을 수 없습니다."));
}

void AR1Monster_LizardWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pattern_Function();
	Impact_PlayerPattern();
}

bool AR1Monster_LizardWarrior::Pattern_Function()
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

void AR1Monster_LizardWarrior::Impact_PlayerPattern()
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

void AR1Monster_LizardWarrior::Setting_Information()
{
	Monster_RegionType = EREGION_TYPE::DEYRON_TYPE;
	MonsterType = EMONSTER_TYPE::LIZARD;
	MonsterName = FText::FromString(TEXT("악어왕"));
	MonsterDocument = FText::FromString(TEXT("강력한 힘과 지혜를 지닌 숲 속의 지배자 입니다."));

	Setting_MonsterAbility(6);
}

bool AR1Monster_LizardWarrior::Setting_Weapon()
{
	FName Soket_Name(TEXT("Left_WeaponSocket"));
	if (GetMesh() && GetMesh()->DoesSocketExist(Soket_Name))
	{
		Left_WeaponMeshComponent->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			Soket_Name
		);
	}
	else
		return false;

	Soket_Name = TEXT("Right_WeaponSocket");
	if (GetMesh() && GetMesh()->DoesSocketExist(Soket_Name))
	{
		Right_WeaponMeshComponent->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			Soket_Name
		);
	}
	else
		return false;

	return true;
}
