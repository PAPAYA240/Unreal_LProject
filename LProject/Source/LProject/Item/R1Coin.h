// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/R1Item.h"
#include "R1Coin.generated.h"

UCLASS()
class LPROJECT_API AR1Coin : public AR1Item
{
	GENERATED_BODY()
public:
	AR1Coin();

	UFUNCTION()
	void CheckLanding();

	void			Set_DropCoin(FVector _target, bool _bFollowCoin = false,  FVector _followTarget = FVector::ZeroVector)
	{ 
		bDrop = true; 
		DropTarget = _target;
		bFollow = _bFollowCoin;
		FollowTarget = _followTarget;
	}

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private :
	bool			CoinDrop(float DeltaTime);
	void			CoinFollow(float DeltaTime);

private :
	bool			bDrop = { false };
	FVector		DropTarget;
	bool			bFollow = { false };
	FVector		FollowTarget;
	
	float			Coin_Timer = { 0.0f };
	float			Coin_FollowTimer = { 0.0f };
	FVector		PrevCoinPos;

private :
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

	double RandomAngle = { 0.f };
	float LaunchSpeed = { 0.f };
	FVector LaunchDirection;

	float InitialSpeed = 600.f; // 초기 속도
	float LaunchAngle = 40.f; // 발사 각도 
	float XYLaunchAngle = 40.f; // 발사 각도 
	float Gravity = -980.f;
	float Time = 0.f; // 시간

	float DropHeightOffset = 50.f; // 발사 시 높이 오프셋

	FTimerHandle LandingCheckTimerHandle;
};
