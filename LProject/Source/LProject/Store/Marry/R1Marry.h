// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Store/R1Quest_Npc.h"
#include "R1Marry.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AR1Marry : public AR1Quest_Npc
{
	GENERATED_BODY()
public:
	AR1Marry();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

};
