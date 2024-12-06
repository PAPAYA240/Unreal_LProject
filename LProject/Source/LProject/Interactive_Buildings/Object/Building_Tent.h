// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_Tent.generated.h"


UCLASS()
class LPROJECT_API ABuilding_Event : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_Event();

protected:
	virtual void BeginPlay() override;
	bool Find_Widget();

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

private :
	void			ChallengeDialog_Type();
	void			RandomCard_Type();

protected :
	// 1. Challenge Dialog Widget
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> ChallengeDialog_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UChallengeDialog_Widet> ChallengeDialog_Instance = { nullptr };

	// 2. Random Card Widget
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> RandomCard_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTreasureCard_Widget> RandomCard_Instance = { nullptr };

private :
	float Stop_Time = { 0.f };
};
