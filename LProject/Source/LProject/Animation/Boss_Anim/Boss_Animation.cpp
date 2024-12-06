// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Boss_Anim/Boss_Animation.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

UBoss_Animation::UBoss_Animation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UBoss_Animation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Find_Player();
}

void UBoss_Animation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!R1Player)
		Find_Player();

	Anim_Pattern();
}

void UBoss_Animation::AnimNotify_End()
{
	bHit = false;
}

void UBoss_Animation::Find_Player()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		R1Player = GameMode->CurrentPlayer;
		if (R1Player)
			CurrentPlayerState = R1Player->Get_CurrentPlayerState();
	}
}

bool UBoss_Animation::Anim_Pattern()
{
	if (CurrentPlayerState)
	{
		if (CurrentPlayerState->TargetMonster)
		{
			if (true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::CUT_SCENE)])
				bCutScene = true;

			// 1. 히든 어택
			/*if (true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
				bHiddenAttack = true;
			else if (false == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
				bHiddenAttack = false;*/

			// 2. 맞기
			if (true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIT)])
				bHit = true;
		}
	}

	return false;
}
