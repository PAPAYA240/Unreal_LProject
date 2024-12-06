// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Monster_Anim/Monster_AnimInstance.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

UMonster_AnimInstance::UMonster_AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> IdleMong(TEXT("/Script/Engine.AnimMontage'/Game/Blueprints/Monster/Montage/MushroomMonster_IdleBlend.MushroomMonster_IdleBlend'"));
	if (IdleMong.Succeeded())
		BattleIdleMontage = IdleMong.Object;
}

void UMonster_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		R1Player = GameMode->CurrentPlayer;
		if (R1Player)
		{
			CurrentPlayerState = R1Player->Get_CurrentPlayerState();
			if(ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
				bBattleStart = true;
		}
	}
}

void UMonster_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (true == bSlowMotion)
		Operate_SlowMotion(DeltaSeconds);
	

	if(R1Monster)
	{
		if(R1Monster == R1Player->Get_CurrentPlayerState()->TargetMonster)
		{
			bBehavior = Anim_Pattern();
			Updated_Mushroom();
		}
	}
}


void UMonster_AnimInstance::AnimNotify_End()
{
	if (R1Monster == R1Player->Get_CurrentPlayerState()->TargetMonster)
	{
		bAttack = false;
		bHit = false;
		bBattleStart = true;
		bHiddenHit = false;

		if (EMONSTER_PATTERN::DEAD == eMonsterPattern)
			R1Monster->Set_FightActiveStop(true);
	}
}

void UMonster_AnimInstance::AnimNotify_Middle()
{
	if (!R1Player || R1Monster != R1Player->Get_CurrentPlayerState()->TargetMonster)
		return;

	if (EMONSTER_PATTERN::DEAD == eMonsterPattern)
		bSlowMotion = true;

	AR1Monster* TargetMonster = R1Player->Get_CurrentPlayerState()->TargetMonster;
	if (true == TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)] ||
		true == TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
		TargetMonster->Set_TimingAttack();

}

void UMonster_AnimInstance::Updated_Mushroom()
{
	if (CurrentPlayerState)
	{
		if (true == CurrentPlayerState->TargetMonster->Get_LookatPlayer() && !bBattleStart)
			BattleIdle_Montage();
	}
}

void UMonster_AnimInstance::Operate_SlowMotion(float DeltaSeconds)
{
	SlowMotionTimer += DeltaSeconds;

	if (SlowMotionTimer >= 0.2f && SlowMotionTimer < 0.5f)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f); 
	}
	else if (SlowMotionTimer >= 0.5f)
	{
		bSlowMotion = false;
		SlowMotionTimer = 0.f;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	}
}

void UMonster_AnimInstance::BattleIdle_Montage()
{
	if (BattleIdleMontage)
		Montage_Play(BattleIdleMontage, 1.0f);
	bBattleStart = true;
}

bool UMonster_AnimInstance::Anim_Pattern()
{
	if (true == bDead)
		return true;

	if (R1Player && CurrentPlayerState)
	{
		if (nullptr != CurrentPlayerState->TargetMonster && R1Monster == CurrentPlayerState->TargetMonster)
		{
			// 죽었을 때는 모든 애니메이션을 무효화 시키고 죽기만 함
			Target_Monster = CurrentPlayerState->TargetMonster;
			if (true == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
			{
				bDead = true;
				eMonsterPattern = EMONSTER_PATTERN::DEAD;
				return true;
			}

			if (true == bBehavior)
			{
				if (false == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(eMonsterPattern)])
				{
					bBehavior = false;
					return false;
				}
				return true;
			}

			if (true == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
			{
				if (true == Target_Monster->Get_DashAndHitStart())
				{
					/* 회피 시 600 거리, 기본 300 거리로 "대쉬 공격 시" 공격 애니메이션 시작*/
					if (300.f >= FVector::Dist(Target_Monster->GetActorLocation(), R1Player->GetActorLocation()) ||
						450.f >= FVector::Dist(Target_Monster->GetActorLocation(), R1Player->GetActorLocation()) && R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
					{
						bRun = false;
						if (false == bBehavior)
						{
							bAttack = true;
							eMonsterPattern = EMONSTER_PATTERN::ATTACK;
							return true;
						}
					}
					else
						bRun = true;
				}
				else
				{
					if (false == bBehavior)
					{
						bAttack = true;
						eMonsterPattern = EMONSTER_PATTERN::ATTACK;
						return true;
					}
				}
			}
			else if (true == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
			{
				if (true == Target_Monster->Get_DashAndHitStart())
				{
					if (300.f >= FVector::Dist(Target_Monster->GetActorLocation(), R1Player->GetActorLocation()) ||
						450.f >= FVector::Dist(Target_Monster->GetActorLocation(), R1Player->GetActorLocation()) && R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
					{
						bRun = false;
						if (false == bBehavior)
						{
							bHiddenHit = true;
							eMonsterPattern = EMONSTER_PATTERN::HIDDEN_ATTACK;
							return true;
						}
					}
					else
						bRun = true;
				}
				else
				{
					if (false == bBehavior)
					{
						bHiddenHit = true;
						eMonsterPattern = EMONSTER_PATTERN::HIDDEN_ATTACK;
						return true;
					}
				}
			}
			else if (true == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIT)])
			{
				if (false == bBehavior)
				{
					bHit = true;
					eMonsterPattern = EMONSTER_PATTERN::HIT;
					return true;
				}
			}

			else if (true == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HILL)])
			{
				if (false == bBehavior)
				{
					bHill = true;
					eMonsterPattern = EMONSTER_PATTERN::HILL;
					return true;
				}
			}

			if (bHill)
			{
				if (false == Target_Monster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HILL)])
					bHill = false;
			}
		}
	}
	return false;
}
