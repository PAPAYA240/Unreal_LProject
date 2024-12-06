// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "FightState_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UFightState_Widget : public UR1UserWidget
{
	GENERATED_BODY()
public:
	UFightState_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private :
	bool			Find_Owner();
	bool			Operate_Text();

	void			Check_PlayerPattern();
	void			Check_MonsterPattern();

protected :
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> CurrentState_Text;

protected :
	UPROPERTY()
	TObjectPtr<class AR1Character> R1Owner;

	UPROPERTY()
	bool bPlayer;

	UPROPERTY()
	bool		bOperate = { false };

	UPROPERTY()
	FVector2D	OriginPos = {};
	
	UPROPERTY()
	float Text_Timer = { 0.f };

	UPROPERTY()
	EPLAYER_PATTERN CurrentPlayer = { EPLAYER_PATTERN::END };

	UPROPERTY()
	EMONSTER_PATTERN CurrentMonster= { EMONSTER_PATTERN::END };
};
