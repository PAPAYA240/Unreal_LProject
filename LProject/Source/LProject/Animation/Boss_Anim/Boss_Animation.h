// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define/R1Define.h"
#include "Animation/AnimInstance.h"
#include "Boss_Animation.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UBoss_Animation : public UAnimInstance
{
	GENERATED_BODY()
public:
	UBoss_Animation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool Anim_Pattern();

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	

private :
	void		Find_Player();

	UFUNCTION()
	void AnimNotify_End();

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;

	UPROPERTY(BlueprintReadOnly)
	bool bBattleStart = false;

	UPROPERTY(BlueprintReadOnly)
	bool bCutScene = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHiddenAttack = false;

protected: // 변수
	TObjectPtr<class AR1Monster>		Target_Monster = { nullptr };
	TObjectPtr<class AR1Player>			R1Player = { nullptr };
	TObjectPtr<class AMyPlayerState>	CurrentPlayerState = { nullptr };

};
