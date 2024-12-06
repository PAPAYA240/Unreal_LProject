// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "R1Npc_Devil.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AR1Npc_Devil : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	AR1Npc_Devil();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

private :
	bool			Setting_Widget();

	void			Floundering();
	void			Operate_Widget();

protected :
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Event_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UBuildingEvent_Widget> Event_Instance;

private :
	UPROPERTY()
	FVector		Origin_Pos			= {};
	UPROPERTY()
	bool			bFloundering		= { false };
};
