// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Define/R1Define.h"
#include "R1AnimInstance.generated.h"


UCLASS()
class LPROJECT_API UR1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public :
	UR1AnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public :
	virtual void NativeInitializeAnimation() override; // 한 번 초기화
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; // Tick의 역할

	UFUNCTION()
	void AnimNotify_End();
	UFUNCTION()
	void AnimNotify_Middle();

private :
	void Find_Player();
	void Movement_Anim();

	bool Anim_Pattern();

protected :
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AR1Character>							Character;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;
	TObjectPtr<class AR1Player> R1Player = { nullptr };
	TObjectPtr<class AMyPlayerState> CurrentPlayerState = { nullptr };

protected :
	UPROPERTY(BlueprintReadOnly)
	FVector m_vVelocity			= FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float m_fGroundSpeed		= 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool m_bShouldMove		= false;

	UPROPERTY(BlueprintReadOnly)
	bool bRun = false;
	UPROPERTY(BlueprintReadOnly)
	bool m_bIsFalling				= false;

	UPROPERTY(BlueprintReadOnly)
	bool bAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHiddenAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHill = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bBattleIdle = false;

	UPROPERTY(BlueprintReadOnly)
	bool bAim = false;

private :
	EPLAYER_PATTERN ePlayerPattern = { EPLAYER_PATTERN::END };

	bool bBehavior = { false };

};
