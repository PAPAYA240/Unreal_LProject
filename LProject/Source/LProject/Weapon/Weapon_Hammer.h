// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/R1Weapon.h"
#include "Define/R1Define.h"
#include "Weapon_Hammer.generated.h"


UCLASS()
class LPROJECT_API AWeapon_Hammer : public AR1Weapon
{
	GENERATED_BODY()
	
public:	
	AWeapon_Hammer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SpawnParticle(EPLAYER_PATTERN _epattern);

};
