#include "Animation/R1AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

UR1AnimInstance::UR1AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR1AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AR1Character>(TryGetPawnOwner());

	if (Character)
		MovementComponent = Character->GetCharacterMovement();

	Find_Player();
}

void UR1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == R1Player)
		Find_Player();

	Movement_Anim();
	bBehavior = Anim_Pattern();
}

void UR1AnimInstance::AnimNotify_End()
{
	bAttack = false;
	bHit = false;
	bHiddenAttack = false;
	bHill = false;

	if (CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)] ||
		CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
	{
		R1Player->Set_EndTimingAttack();
	}
}

void UR1AnimInstance::AnimNotify_Middle()
{
	if (!CurrentPlayerState || !R1Player)
		return;

	/* Attack일 때 Particle을 위해서 때리는 타이밍을 알아야 함*/
	if (CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)] ||
		CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
	{
		R1Player->Set_TimingAttack();
	}
}

void UR1AnimInstance::Find_Player()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		R1Player = GameMode->CurrentPlayer;
		if (R1Player)
		{
			CurrentPlayerState = R1Player->Get_CurrentPlayerState();
			if(CurrentPlayerState)
			{
				if (ESCENE::FIGHT_SCENE == R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
				{
					bBattleIdle = true;
				}
			}
		}
	}

}

void UR1AnimInstance::Movement_Anim()
{
	if (nullptr == Character || nullptr == MovementComponent)
		return;

	m_vVelocity = MovementComponent->Velocity;
	m_fGroundSpeed = m_vVelocity.Size2D();

	m_bShouldMove = (m_fGroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
	m_bIsFalling = MovementComponent->IsFalling();

}

bool UR1AnimInstance::Anim_Pattern()
{
	if (nullptr == R1Player || nullptr == CurrentPlayerState)
		return false;

	AR1Player* TargetPlayer = { nullptr };
	if(ESCENE::BOSS_SCENE != CurrentPlayerState->Player_CurrentScene)
	{
		TargetPlayer = R1Player;
	}
	else
	{
		TargetPlayer = Cast<AR1Player>(Character);
	}

	if (nullptr == TargetPlayer || nullptr == TargetPlayer->Get_CurrentPlayerState())
		return false;

	if (true == bDead)
		return true;

	if (true == bBehavior)
	{
		if (false == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(ePlayerPattern)])
		{
			bBehavior = false;
			return false;
		}
		return true;
	}

	if (true == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)])
	{
		if (false == bBehavior)
		{
			bDead = true;
			ePlayerPattern = EPLAYER_PATTERN::DEAD;
			return true;
		}
	}
	else if (true == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)])
	{
		if (false == bBehavior)
		{
			bAim = true;
			ePlayerPattern = EPLAYER_PATTERN::AIM;
			return true;
		}
	}
	else if (true == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
	{
		if (true == R1Player->IsNearbyAttack())
		{
			AR1Monster* Target_Monster = TargetPlayer->Get_CurrentPlayerState()->TargetMonster;
			if (!Target_Monster)
				return false;
			/* 회피 시 600 거리, 기본 300 거리로 "대쉬 공격 시" 공격 애니메이션 시작*/
			if (450.f >= FVector::Dist(Target_Monster->GetActorLocation(), R1Player->GetActorLocation()))
			{
				bRun = false;
				if (false == bBehavior)
				{
					bAttack = true;
					ePlayerPattern = EPLAYER_PATTERN::ATTACK;
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
				ePlayerPattern = EPLAYER_PATTERN::ATTACK;
				return true;
			}
		}
	}
	else if (true == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
	{
		if (false == bBehavior)
		{
			bHiddenAttack = true;
			ePlayerPattern = EPLAYER_PATTERN::HIDDEN_ATTACK;
			return true;
		}
	}
	else if (true == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)])
	{
		if (false == bBehavior)
		{
			bHit = true;
			ePlayerPattern = EPLAYER_PATTERN::HIT;
			return true;
		}
	}
	else if (true == TargetPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HILL)])
	{
		if (false == bBehavior)
		{
			bHill = true;
			ePlayerPattern = EPLAYER_PATTERN::HILL;
			return true;
		}
	}

	return false;
}
