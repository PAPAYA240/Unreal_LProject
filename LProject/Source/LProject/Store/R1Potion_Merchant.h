// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Store/R1Npc.h"
#include "R1Potion_Merchant.generated.h"


UCLASS()
class LPROJECT_API AR1Potion_Merchant : public AR1Npc
{
	GENERATED_BODY()

public:
	AR1Potion_Merchant();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;


};
