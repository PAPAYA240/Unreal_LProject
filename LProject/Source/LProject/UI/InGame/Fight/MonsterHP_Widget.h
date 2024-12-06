// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "MonsterHP_Widget.generated.h"

UCLASS()
class LPROJECT_API UMonsterHP_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UMonsterHP_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public : //============= For. Getter Setter =============
	void Set_TargetMonster(class AR1Monster* _monster);
	void Set_ProgressBar(float _percent);
	float Get_ProgressBar();
	bool Get_EndOperateVS() { return bVSEnd; }

	void Set_HpText(float _maxHp, float _hp);
	void Set_AttackText(float _attack);
	void Set_LevelText(float _level);

private ://============= For. 함수 =============
	bool		Setting_WidgetOption();

	bool		Opreate_VSImage(); // VS Image의 움직임 관련
	void		Operate_BossMap();
	void		Operate_FightScene();

protected ://============= For. Widget 함수 =============
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UProgressBar> ProgressBar;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> MonsterName_Text;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Level_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> VS_Image;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Hp_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Attack_Text;

private ://============= For. 변수 =============
	TObjectPtr<class AR1Monster> R1Monster = { nullptr };

	UPROPERTY()
	FVector2D	VS_OriginScaled = {};

	UPROPERTY()
	FVector2D	VS_OriginPos= {};

	UPROPERTY()
	float			VS_Timer = { 0.f };
	UPROPERTY()
	bool			bScaledChange = { false };

	UPROPERTY()
	bool			bVSEnd= { false };

	UPROPERTY()
	bool			bFightStart = { false };
};
