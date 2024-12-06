

#include "Player/Player_Wizard/R1Player_Wizard.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraManager.h"

AR1Player_Wizard::AR1Player_Wizard()
	: Super()
{
	/* Attack_Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem1(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_SkullCrash_01.Par_LoPo_SkullCrash_01'"));
	if (ParticleSystem1.Succeeded())
	{
		Attack_Particle = Cast<UParticleSystem>(ParticleSystem1.Object);
	}

	/* Hidden_Particle */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem2(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Skull_03.Par_LoPo_Skull_03'"));
	if (ParticleSystem2.Succeeded())
	{
		Hidden_Particle = Cast<UParticleSystem>(ParticleSystem2.Object);
	}

	// Aim Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Spark_04.Par_LoPo_Spark_04'"));
	if (ParticleSystem.Succeeded())
		AimAttack_Particle = ParticleSystem.Object;
}

void AR1Player_Wizard::BeginPlay()
{
	TargetAttack = CurrentAttack = 40;
	TargetHill = FinalHill = 10;
	bNearbyAttack = false; 

	Super::BeginPlay();

	if (!Setting_PlayerMaterial())
		MSG_BOX(TEXT("Player Material이 세팅되지 않았습니다."));

	Setting_Weapon();
}

void AR1Player_Wizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Impact_MonsterPattern();
	Pattern_Function();
}

void AR1Player_Wizard::Pattern_Function()
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
			if (false == bIsShirkJumping)
				CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)] = false;
			Super::StopJumping();
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
		if (TimingAttack())
		{
			if (Attack_Particle)
			{
				FVector TargetLoc = CurrentPlayerState->TargetMonster->GetActorLocation();
				TargetLoc.Z -= 88.;
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Attack_Particle, TargetLoc, this->GetActorRotation());
				bFinishAttack = true;
			}
			Stop_TimingAttack();
		}
		
		if(true == bFinishAttack)
		{
			if(StopAttack(EPLAYER_PATTERN::ATTACK))
				bFinishAttack = false;
		}
	}

	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
	{
		if (TimingAttack())
		{
			if (false == bFinishAttack)
			{
				if (Hidden_Particle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Hidden_Particle, this->GetActorLocation(), this->GetActorRotation());
					bFinishAttack = true;
					GetMesh()->SetVisibility(false);
				}
				Stop_TimingAttack();
			}
		}

		if (true == bFinishAttack)
		{
			if(StopAttack(EPLAYER_PATTERN::HIDDEN_ATTACK))
			{
				bFinishAttack = false;

				GetMesh()->SetVisibility(true);
				FVector CurrentPos = this->GetActorLocation();
				CurrentPos.Z += 1000.f;
				this->SetActorLocation(CurrentPos);
			}
		}
	}

	else if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)])
	{
		Start_PullBack();
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

void AR1Player_Wizard::Setting_Weapon()
{
}

bool AR1Player_Wizard::Setting_PlayerMaterial()
{
	PlayerMaterial_Cnt[0] = CUSTOMIZE_PLAYER::FACE_CUSTOM;
	PlayerMaterial_Cnt[1] = CUSTOMIZE_PLAYER::_CUSTOM;
	PlayerMaterial_Cnt[2] = CUSTOMIZE_PLAYER::TOB_CUSTOM;
	PlayerMaterial_Cnt[3] = CUSTOMIZE_PLAYER::HAIR_CUSTOM;
	PlayerMaterial_Cnt[4] = CUSTOMIZE_PLAYER::HAIR_CUSTOM;
	PlayerMaterial_Cnt[5] = CUSTOMIZE_PLAYER::UNDERWARE_CUSTOM;

	return true;
}

