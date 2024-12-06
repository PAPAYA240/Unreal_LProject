// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/R1Weapon.h"
#include "Weapon_ElectricBall.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AWeapon_ElectricBall : public AR1Weapon
{
	GENERATED_BODY()
public:
	AWeapon_ElectricBall();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	bool			Fire_Ball(class AR1Character* _target);

	UPROPERTY()
	float FireSpeed = 5.0f;

	UPROPERTY()
	bool bParticleStart = { false };

	UPROPERTY()
	FVector Final_Posiiton = { };

//	float Timer = { 0.f };
};
