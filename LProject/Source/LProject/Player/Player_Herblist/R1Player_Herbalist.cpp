

#include "Player/Player_Herblist/R1Player_Herbalist.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "R1GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

#include "Weapon/Weapon_Sickle.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraManager.h"

#include "UI/InGame/Ability/HerblistAbility_Widget.h"

AR1Player_Herbalist::AR1Player_Herbalist()
	: Super()
{
	// Weapon
	ConstructorHelpers::FClassFinder<AActor> WeaponBP(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Weapon/BP_Sickle.BP_Sickle_C'"));
	if (WeaponBP.Succeeded() && WeaponBP.Class)
	{
		Find_Weapon = WeaponBP.Class;
	}

	// Aim Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Spark_01.Par_LoPo_Spark_01'"));
	if (ParticleSystem.Succeeded())
		AimAttack_Particle = ParticleSystem.Object;
}

void AR1Player_Herbalist::BeginPlay()
{
	TargetAttack = CurrentAttack = 20;
	TargetHill = FinalHill = 30;
	bNearbyAttack = true;

	Super::BeginPlay();

	if (!Setting_PlayerMaterial())
		MSG_BOX(TEXT("AR1Player_Herbalist() : Player Material이 세팅되지 않았습니다."));

	if(!Setting_Widget())
		MSG_BOX(TEXT("AR1Player_Herbalist() : Widget이 세팅되지 않았습니다."));

	if (!Setting_Weapon())
		MSG_BOX(TEXT("AR1Player_Herbalist() : Weapon이 세팅되지 않았습니다."));
}

void AR1Player_Herbalist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Impact_MonsterPattern();
	Pattern_Function();
}

void AR1Player_Herbalist::Pattern_Function()
{
	if (nullptr == CurrentPlayerState)
		return;

	if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
		return;

	// PlayerPattern
	if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
	{
		if (false == ShirkJump())
		{
			Super::StopJumping();
			if (false == bIsShirkJumping)
				CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)] = false;
		}
	}
	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM_ATTACK)])
	{
		if (AimAttack_Particle)
		{
			if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
			{
				FRotator SpawnRotation = MyCameraManager->Get_CameraActor()->GetComponentRotation();
				FVector SpawnLocation = MyCameraManager->Get_CameraActor()->GetComponentLocation();
				FVector ForwardVector = SpawnRotation.Vector();
				SpawnLocation += ForwardVector * 300.f;

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AimAttack_Particle, SpawnLocation, SpawnRotation);

				CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM_ATTACK)] = false;
			}
		}
	}
	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
	{
		if (TimingAttack()) /* 타이밍에 맞게 파티클 호출*/
		{
			if (Player_Weapon)
				Cast<AWeapon_Sickle>(Player_Weapon)->SpawnParticle(EPLAYER_PATTERN::ATTACK);
			Stop_TimingAttack();
		}

		if (true == HitAndDash(this, CurrentPlayerState->TargetMonster->Get_InitPos(), true)) /* 대시가 끝났다면*/
		{
			if (true == Ready_NextTurn(EPLAYER_PATTERN::ATTACK, EFIGHT_AUTHORITY::MONSTER))
			{
				Stop_HitAndDash();
				StopAttack(EPLAYER_PATTERN::HIDDEN_ATTACK);
			}
		}
	}

	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
	{
		if (TimingAttack()) /* 타이밍에 맞게 파티클 호출*/
		{
			if (Player_Weapon)
				Cast<AWeapon_Sickle>(Player_Weapon)->SpawnParticle(EPLAYER_PATTERN::HIDDEN_ATTACK);
			Stop_TimingAttack();
		}
		else
		{
			HiddenAttackEnd_Timer += GetWorld()->GetDeltaSeconds();
			if(1.5f <= HiddenAttackEnd_Timer)
			{
				if(StopAttack(EPLAYER_PATTERN::HIDDEN_ATTACK))
					HiddenAttackEnd_Timer = 0.f;
			}
		}
	}

	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)])
	{
		if (true == Hit())
		{
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = false;
		}
	}
	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)])
	{
		bUseControllerRotationYaw = true;
	}
}

/*
void AR1Player_Herbalist::Impact_MonsterPattern()
{
	// Monster의 영향
	if (!CurrentPlayerState || !CurrentPlayerState->TargetMonster)
		return;

	if (ESCENE::BOSS_SCENE != CurrentPlayerState->Player_CurrentScene)
	{
		if (true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] ||
			true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
		{
			if (EIMPACT::IMPACT_ATTACK != IsImpact)
			{
				IsImpact = EIMPACT::IMPACT_ATTACK;

				if (true == CalculateAbility((ABILITY_TYPE::DEFENCE)))
				{
					MSG_BOX(TEXT("회피 성공"));
					CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)] = true;
				}
				else
				{
					CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)] = true;
					//CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = true;
					MSG_BOX(TEXT("회피 실패"));
				}
			}
		}
		else
			IsImpact = EIMPACT::IMPACT_END;
	}
}
*/

bool AR1Player_Herbalist::Setting_Widget()
{
	HerblistAility_Instance = Cast<UHerblistAbility_Widget>(CreateWidget(GetWorld(), HerblistAility_Widget));
	if (HerblistAility_Instance)
	{
		HerblistAility_Instance->AddToViewport();
		HerblistAility_Instance->Set_OwnerPlayer(this);
	}
	else
		return false;
	return true;
}

bool AR1Player_Herbalist::Setting_Weapon()
{
	/* 무기 생성 */
	if (Find_Weapon)
	{
		AWeapon_Sickle* WeaponActor = GetWorld()->SpawnActor<AWeapon_Sickle>(Find_Weapon, GetActorLocation(), GetActorRotation());
		if (WeaponActor)
		{
			if (GetMesh())
			{
				WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r_Soket"));
				Player_Weapon = WeaponActor;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
	return true;
}

bool AR1Player_Herbalist::Setting_PlayerMaterial()
{
	PlayerMaterial_Cnt[0] = CUSTOMIZE_PLAYER::FACE_CUSTOM;
	PlayerMaterial_Cnt[1] = CUSTOMIZE_PLAYER::_CUSTOM;
	PlayerMaterial_Cnt[2] = CUSTOMIZE_PLAYER::TOB_CUSTOM;
	PlayerMaterial_Cnt[3] = CUSTOMIZE_PLAYER::HAIR_CUSTOM;
	PlayerMaterial_Cnt[4] = CUSTOMIZE_PLAYER::HAIR_CUSTOM;
	PlayerMaterial_Cnt[5] = CUSTOMIZE_PLAYER::UNDERWARE_CUSTOM;

	return true;
}
