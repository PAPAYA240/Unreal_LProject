// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/R1AnimInstance.h"
#include "R1MerchantAnim.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UR1MerchantAnim : public UR1AnimInstance
{
	GENERATED_BODY()
public:
	UR1MerchantAnim(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected :
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	bool bWalk = false;

	UPROPERTY(BlueprintReadOnly)
	bool bStoreOpen = false;
};
