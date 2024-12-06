// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/R1Weapon.h"
#include "Weapon_Sickle.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AWeapon_Sickle : public AR1Weapon
{
	GENERATED_BODY()
public:
	AWeapon_Sickle();


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void SpawnParticle(EPLAYER_PATTERN _epattern);
};
