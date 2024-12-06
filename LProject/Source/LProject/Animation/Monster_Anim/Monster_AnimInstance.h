// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Define/R1Define.h"
#include "Monster_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UMonster_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UMonster_AnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool Anim_Pattern();

public :
	void Set_Monster(class AR1Monster* _monster) { R1Monster = _monster;  }

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void BattleIdle_Montage();
	UFUNCTION()
	void AnimNotify_End();
	UFUNCTION()
	void AnimNotify_Middle();

private :
	void Updated_Mushroom();
	void Operate_SlowMotion(float DeltaSeconds); // 몬스터가 사망했을 때 슬로우 모션 재생

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bAttack		= false;

	UPROPERTY(BlueprintReadOnly)
	bool bHit			= false;

	UPROPERTY(BlueprintReadOnly)
	bool bHiddenHit  = false;

	UPROPERTY(BlueprintReadOnly)
	bool bDead			= false;

	UPROPERTY(BlueprintReadOnly)
	bool bBattleStart	= false;

	UPROPERTY(BlueprintReadOnly)
	bool bCutScene	= false;

	UPROPERTY(BlueprintReadOnly)
	bool bFailedHit		= false;

	UPROPERTY(BlueprintReadOnly)
	bool bVictory		= false;

	UPROPERTY(BlueprintReadOnly)
	bool bHill			= false;

	UPROPERTY(BlueprintReadOnly)
	bool bRun			= false;

protected : // 몽타주
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* BattleIdleMontage;

protected : // 변수
	TObjectPtr<class AR1Monster>		Target_Monster = { nullptr };
	TObjectPtr<class AR1Monster>		R1Monster  = { nullptr };
	TObjectPtr<class AR1Player>			R1Player = { nullptr };
	TObjectPtr<class AMyPlayerState>	CurrentPlayerState = { nullptr };

	bool bBehavior = { false };
	EMONSTER_PATTERN eMonsterPattern = { EMONSTER_PATTERN::END };

	bool bSlowMotion = { false };
	float SlowMotionTimer = { 0.f };
};
