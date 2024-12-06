// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "R1RobbyModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AR1RobbyModeBase : public AGameModeBase
{
	GENERATED_BODY()

public : 
	AR1RobbyModeBase();


public:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

};
