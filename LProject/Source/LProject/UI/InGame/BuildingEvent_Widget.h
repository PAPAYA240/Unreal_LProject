// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "BuildingEvent_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UBuildingEvent_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UBuildingEvent_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	bool Get_Pray() { return bPlayRandom; }
	void Set_Pray(bool _pray) { bPlayRandom = _pray;  }

	void Setting_WidgetText(EBUILDING_TYPE _building);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnPrayClicked();

	UFUNCTION(BlueprintCallable)
	void OnExitClicked();

protected  :
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Prayer_Button;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Exit_Button;
	
private :
	bool					bPlayRandom			= { false };
	EBUILDING_TYPE  Building_Type			= { EBUILDING_TYPE::END_BUILDING };

};
