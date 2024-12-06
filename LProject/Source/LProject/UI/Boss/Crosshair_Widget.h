// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "Crosshair_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UCrosshair_Widget : public UR1UserWidget
{
	GENERATED_BODY()
	
public:
	UCrosshair_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Crosshair_GameStart(bool _start);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
};
