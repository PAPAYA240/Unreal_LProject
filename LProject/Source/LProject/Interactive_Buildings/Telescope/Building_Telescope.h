// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_Telescope.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API ABuilding_Telescope : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_Telescope();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

public:
	void		Set_Operate(bool _operate) { bOperate = _operate; }

private :
	void		OpenBossMap();
	void		Operate_BossMap();

private :
	float			ChangeLevel_Timer	= { 0.f };
	bool			bOperate				= { false };
	bool			bInitPos					= { false };
	FVector		BT_InitPos				= {};

};
