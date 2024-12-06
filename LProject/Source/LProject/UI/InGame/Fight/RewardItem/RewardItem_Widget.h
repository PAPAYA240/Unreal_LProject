// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/R1ItemData.h"
#include "UI/R1UserWidget.h"
#include "RewardItem_Widget.generated.h"


UCLASS()
class LPROJECT_API URewardItem_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	URewardItem_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);


protected : // ========== For. 델리게이트 ==========
	UFUNCTION(BlueprintCallable)
	void OnGet_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnGetOut_Clicked();


private : // ============= For. 함수 =============
	bool Operate_RewardWidget();
	void Find_ItemType();
	bool Operate_GetIemWidget();
	void Operate_NextBackground();


protected :// ============= For. Widget 변수 =============
	/* "획득하기" 버튼*/
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Get_Button;

	/* "버리기 버튼*/
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> GetOut_Button;
	
	// 아이템 Name
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> ItemName_Text;

	// 아이템 Image
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Item_Image;

	// Background
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> BackGround;

	// 다음 Scene으로 넘어갈 때 사용되는 Background
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Next_BackGround;
	


private :// ============= For. 변수 =============
	// 아이템 정보
	FItemData_Asset*		Item_Asset = { nullptr };
	UPROPERTY()
	FVector2D				Canvas_initPos;
	UPROPERTY()
	FVector2D				NextBG_initPos;
	UPROPERTY()
	bool						bOperateWidget = { false };
	UPROPERTY()
	int32						CurrentEatable_ItemCount = { };
	UPROPERTY()
	int32						Eatable_ItemCount = { 2 };
	UPROPERTY()
	bool						EatableItem_End = { false };

};
