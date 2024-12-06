// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

UCLASS()
class LPROJECT_API ATrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrap();

protected:
	virtual void BeginPlay() override;
	  UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public :
	virtual void Tick(float DeltaTime) override;
	
private :
	bool Find_Player();

protected :
	UPROPERTY()
	TObjectPtr<class AR1Player> R1Player;

	UPROPERTY()
	TObjectPtr<class AMyPlayerState> CurrentPlayerState;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> uStaticMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

public :
	float FinalDamage = 5.f;

};
