

#include "Player/Player_Lumberjack/R1Player_Lumberjack.h"
#include "Player/MyPlayerState.h"
#include "R1Player_Lumberjack.h"

#include "Weapon/R1Weapon.h"
#include "Weapon/Weapon_Hammer.h"

#include "R1GameModeBase.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraManager.h"

#include "Monster/R1Monster.h"

AR1Player_Lumberjack::AR1Player_Lumberjack()
	: Super()
{
	// Weapon
	ConstructorHelpers::FClassFinder<AActor> WeaponBP(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Weapon/BP_Hammer.BP_Hammer_C'"));
	if (WeaponBP.Succeeded() && WeaponBP.Class)
	{
		Find_Weapon = WeaponBP.Class;
	}

	// Aim Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Spark_02.Par_LoPo_Spark_02'"));
	if (ParticleSystem.Succeeded())
		AimAttack_Particle = ParticleSystem.Object;
}

void AR1Player_Lumberjack::BeginPlay()
{
	TargetAttack = CurrentAttack = 40;
	TargetHill = FinalHill = 10;
	bNearbyAttack = true;

	Super::BeginPlay();

	if (!Setting_PlayerMaterial())
		MSG_BOX(TEXT("Player Material이 세팅되지 않았습니다."));

	Setting_Weapon();
}

void AR1Player_Lumberjack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Impact_MonsterPattern();
	Pattern_Function();
}

void AR1Player_Lumberjack::Pattern_Function()
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
			if(false == bIsShirkJumping)
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
				Cast<AWeapon_Hammer>(Player_Weapon)->SpawnParticle(EPLAYER_PATTERN::ATTACK);
			Stop_TimingAttack();
		}

		if (nullptr == CurrentPlayerState->TargetMonster)
			return;

		if (true == HitAndDash(this, CurrentPlayerState->TargetMonster->Get_InitPos())) /* 대시가 끝났다면*/
		{
			if (true == Ready_NextTurn(EPLAYER_PATTERN::ATTACK, EFIGHT_AUTHORITY::MONSTER))
				Stop_HitAndDash();
		}
	}

	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
	{
		if (TimingAttack())
		{
			if(Player_Weapon)
				Cast<AWeapon_Hammer>(Player_Weapon)->SpawnParticle(EPLAYER_PATTERN::HIDDEN_ATTACK);
			Stop_TimingAttack();
		}
		else
			StopAttack(EPLAYER_PATTERN::HIDDEN_ATTACK);
	}

	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)])
	{
		if(ESCENE::BOSS_SCENE != CurrentPlayerState->Player_CurrentScene)
		{
			Start_PullBack();
		}

		if (true == Hit())
		{	
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = false;
			bPullBack_End = false;
		}
	}
	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)])
	{
		bUseControllerRotationYaw = true;
	}


	// 밀치기
	if (true == bPullBack_Start)
		PullBack();
}

bool AR1Player_Lumberjack::Setting_PlayerMaterial()
{
	for (int i = 0; i < static_cast<int>(CUSTOMIZE_PLAYER::END_CUSTOM); i++)
	{
		PlayerMaterial_Cnt[i] = static_cast<CUSTOMIZE_PLAYER>(i);
	}
	return true;
}

void AR1Player_Lumberjack::Setting_Weapon()
{
	/* 무기 생성 */
	if(Find_Weapon)
	{
		AWeapon_Hammer* WeaponActor = GetWorld()->SpawnActor<AWeapon_Hammer>(Find_Weapon, GetActorLocation(), GetActorRotation());
		if (WeaponActor)
		{
			if (GetMesh())
			{
				WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r_Soket"));
				Player_Weapon = WeaponActor;
			}
		}
	}
}

