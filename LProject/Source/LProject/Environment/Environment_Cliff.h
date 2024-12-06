// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment_Cliff.generated.h"

UCLASS()
class LPROJECT_API AEnvironment_Cliff : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnvironment_Cliff();

	void Set_Upper_Cliff(bool _upper) { bMoving_cliff = _upper; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private :
	bool Find_Player();

protected :
	class AR1Player* R1Player;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> uStaticMesh;

	bool bPlayerPos = { false };
	bool bMoving_cliff = { false };
	FVector Init_Pos;
	FVector PlayerTarget_Pos;
};
