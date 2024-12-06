// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Boss/Crosshair_Widget.h"

UCrosshair_Widget::UCrosshair_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCrosshair_Widget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCrosshair_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCrosshair_Widget::Crosshair_GameStart(bool _start)
{
	if (true == _start)
	{
		Set_ShowWidget(true);

	}
}
