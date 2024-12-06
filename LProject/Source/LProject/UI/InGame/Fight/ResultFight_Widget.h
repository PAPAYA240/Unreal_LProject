// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "ResultFight_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UResultFight_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UResultFight_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);


public  : //============= For. Getter Setter =============
	void Set_CallResultWidget(bool _call){ bOperateWidget = _call; } // 직접적으로 Widget을 불러와야 할 때 사용


private : //============= For. 함수=============
	void Image_Moving(float DeltaTime);
	void Fight_SceneWidget();
	void InGame_SceneWidget();


protected: //============= For. Widget 함수 =============
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Left_Image;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Right_Image;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Result_Text;


protected://============= For. Sound  =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* WinSound;

private : //============= For. 변수 =============
	UPROPERTY()
	bool						bOperateWidget = { false };
	UPROPERTY()
	FVector2D				Left_InitPos;
	UPROPERTY()
	FVector2D				Right_InitPos;
	UPROPERTY()
	FVector2D				Left_TargetPos;
	UPROPERTY()
	FVector2D				Right_TargetPos;
	UPROPERTY()
	bool						bBetweenTrum = { false };

};
