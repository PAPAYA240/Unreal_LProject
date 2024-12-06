// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "RandomBox_Widget.generated.h"

UENUM(BlueprintType)
enum class ERANDOMBOX_TYPE : uint8
{
	FULL_ENERGY,
	GET_BOOK,
	GET_ATTACKPOTION,
	ADD1_ENERGY,
	MINUS15_ATTACK,
	FULL_HP,
	ADD500_COIN,
	GET_TELEPORT_SCROLL,
	GET_ONE_DEAD,
	MINUS100_COIN,
	ADD20_HP,
	MINUS25_HP,
	END_RANDOM_TYPE
};


UCLASS()
class LPROJECT_API URandomBox_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	URandomBox_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void			ClickStartOption();

private :
	bool			WidgetOption_Setting();
	void			Opreate_RandomBox();

	bool			Select_Reward();

private :
	TArray<class UImage*> ItemImage_Array;

	UPROPERTY()
	bool				bStart = { false };

	UPROPERTY()
	int32				SelectItemCnt = { 0 };

	UPROPERTY()
	float				RandomBox_Timer = { 0.f };
	UPROPERTY()
	float				RandomSpeed = { 0.f };
	UPROPERTY()
	float				CurrentRandomSpeed = { 0.f };
	UPROPERTY()
	float				SelectItem_Timer = { 0.f };
	UPROPERTY()
	bool				bSelectColor = { false };
	UPROPERTY()
	ERANDOMBOX_TYPE Reward_Type = { ERANDOMBOX_TYPE::END_RANDOM_TYPE };
};
