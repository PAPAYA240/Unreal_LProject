// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/R1Highlight_Interface.h"
#include "GameFramework/Actor.h"
#include "Environment_Floor.generated.h"

UCLASS()
class LPROJECT_API AEnvironment_Floor : public AActor, public IR1Highlight_Interface
{
	GENERATED_BODY()
	
public:	
	AEnvironment_Floor();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Highlight() override;
	virtual void UnHighlight() override;

private:
	bool Find_Player();

protected:
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	class UBoxComponent* TriggerBox;

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BoxMesh;

protected:
	TObjectPtr<class AR1Player> R1Player;

	TObjectPtr<UStaticMeshComponent> uStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "uStaticMesh")
	UStaticMesh* MeshAsset;

private :
	TObjectPtr<class AThorns_Trap> FloorTrap = { nullptr };

	bool bHighlighted			= { false };

	FVector Init_Pos;
	FVector Target_Pos;
	bool bOverlapSuccess	= { false };
	bool bOverlap				= { false };

};
