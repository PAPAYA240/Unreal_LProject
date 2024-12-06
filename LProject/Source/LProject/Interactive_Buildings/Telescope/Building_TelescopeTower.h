// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_TelescopeTower.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API ABuilding_TelescopeTower : public AInteractive_Buildings
{
	GENERATED_BODY()
public:
	ABuilding_TelescopeTower();

	FVector Get_Center() { return Init_Position;  }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

private :
	void Operate_Building();

public:
	virtual void Tick(float DeltaTime) override;

private :
	float Operate_Timer = { 0.f };
	FVector Init_Position;
};
