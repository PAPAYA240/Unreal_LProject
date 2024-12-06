// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/R1Player.h"
#include "R1Player_Wizard.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AR1Player_Wizard : public AR1Player
{
	GENERATED_BODY()
public:
	AR1Player_Wizard();

protected:
	void BeginPlay();
	virtual void Pattern_Function();

public:
	void Tick(float DeltaTime);


private:
	void Setting_Weapon();
	bool Setting_PlayerMaterial();

private :
	UPROPERTY()
	class UParticleSystem* Attack_Particle;

	UPROPERTY()
	class UParticleSystem* Hidden_Particle;

	bool		bFinishAttack = { false };
	bool		bHiddenSetting = { false };
};
