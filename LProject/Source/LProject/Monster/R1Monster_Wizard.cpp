// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/R1Monster_Wizard.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Particles/ParticleSystem.h" 

#include "Camera/CameraManager.h"

AR1Monster_Wizard::AR1Monster_Wizard()
	: Super()
{
	if (false == Setting_Particle())
		MSG_BOX(TEXT("이펙트 생성 불가 : AR1Monster_Wizard"))
}

void AR1Monster_Wizard::BeginPlay()
{
	MonsterType = EMONSTER_TYPE::WIZARD;
	Super::BeginPlay();
}

void AR1Monster_Wizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
   
	if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
		CurrentPlayerState->TargetMonster = this;

	Impact_PlayerPattern(); // 행동을 정해줌
	Pattern_Function();  // 행동을 실행함
}

bool AR1Monster_Wizard::Pattern_Function()
{
	if (nullptr == CurrentPlayerState)
		return false;

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)])
	{
		Hill();
		if (true == ParticleStart(Hill_Particle, true, 2.f, this->GetActorLocation()))
		{
			Ready_NextTurn(EMONSTER_PATTERN::HILL, EFIGHT_AUTHORITY::PLAYER);
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
	{
		if (true == ParticleStart(Attack_Particle, false, 0.1f, this->GetActorLocation(), 7))
		{
			Ready_NextTurn(EMONSTER_PATTERN::ATTACK, EFIGHT_AUTHORITY::PLAYER);
		}
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
    {
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
    }
	
	/*else if (EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
	{
		if (false == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
		{
			FVector MonsterPosition = GetActorLocation();
			int32 NumberOfBalls = ElectricBall_Array.Num();

			float fCurrent_SpawneeAngle = 0.f;
			float fAngle = 360.f / NumberOfBalls;
			const float Radius = 300.0f;

			for (int32 i = 0; i < NumberOfBalls; ++i)
			{
				FVector vFinalLoc = MonsterPosition + FVector(
					FMath::Cos(FMath::DegreesToRadians(fCurrent_SpawneeAngle)) * Radius,
					FMath::Sin(FMath::DegreesToRadians(fCurrent_SpawneeAngle)) * Radius,
					0.f
				);

				// 전기 볼의 최종 위치 설정
				ElectricBall_Array[i]->Final_Posiiton = vFinalLoc;
				ElectricBall_Array[i]->SetActorLocation(vFinalLoc); // 즉시 위치 설정

				fCurrent_SpawneeAngle += fAngle;
			}

			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEFAULT)] = false;
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = true;
		}
		else
		{
			bool bAllPass = true;

			for (int i = 0; i < ElectricBall_Array.Num(); i++)
			{
				FVector CurrentLocation = ElectricBall_Array[i]->GetActorLocation();
				FVector TargetLocation = ElectricBall_Array[i]->Final_Posiiton;
				const float Speed = 200.0f;

				if (false == bPlayer_Attack)
				{
					if (FVector::Dist(CurrentLocation, TargetLocation) > 10.0f)
					{
						FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
						FVector NewLocation = CurrentLocation + (Direction * Speed * GetWorld()->GetDeltaSeconds());
						ElectricBall_Array[i]->SetActorLocation(NewLocation);
					}
					else
						bPlayer_Attack = true;

					return;
				}
				else if (true == bPlayer_Attack)
				{
					MonsterAttack_Timer += GetWorld()->GetDeltaSeconds();

					if (FVector::Dist(CurrentLocation, R1Player->GetActorLocation()) > 10.0f)
					{
						if (MonsterAttack_Timer > 1.f)
						{
							FVector Direction = (R1Player->GetActorLocation() - CurrentLocation).GetSafeNormal();
							FVector NewLocation = CurrentLocation + (Direction * Speed * GetWorld()->GetDeltaSeconds());
							ElectricBall_Array[i]->SetActorLocation(NewLocation); // 최종 위치로 설정
						}

						bAllPass = false;
					}
				}
			}

            if (true == bAllPass)
            {
                MonsterAttack_Timer = 0.f;
                bPlayer_Attack = false;

                Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEFAULT)] = true;
                Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = false;

                for (int i = 0; i < ElectricBall_Array.Num(); i++)
                    ElectricBall_Array[i]->SetActorLocation(this->GetActorLocation());
            }
        }
    }*/
	return true;
}

void AR1Monster_Wizard::Impact_PlayerPattern()
{
	if (!CurrentPlayerState)
		return;

	// 컷신 중일 때
	if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene &&
		ESCENE::LOBBY == CurrentPlayerState->Player_PrevScene)
	{
		ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);

		if (MyCameraManager->Get_CutSceneAnimStart())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::CUT_SCENE)] = true;
	}

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

bool AR1Monster_Wizard::Setting_Particle()
{
	/* Hill */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Hill_ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/FXVarietyPack/Particles/P_ky_healAura.P_ky_healAura'"));
	if (Hill_ParticleSystem.Succeeded())
		Hill_Particle = Hill_ParticleSystem.Object;
	else
		return false;

	/* Attack */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Attack_ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/FXVarietyPack/Particles/P_ky_waterBall.P_ky_waterBall'"));
	if (Attack_ParticleSystem.Succeeded())
		Attack_Particle = Attack_ParticleSystem.Object;
	else
		return false;

	return true;
}
