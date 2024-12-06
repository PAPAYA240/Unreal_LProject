// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/R1ItemData.h"
#include "UI/R1UserWidget.h"
#include "R1Merchant_ItemInfo.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UR1Merchant_ItemInfo : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UR1Merchant_ItemInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Set_VerticalPtr(class UVerticalBox* _vert) { Verticle_Canvas = _vert; }
	void Set_ItemInfo(FItemData_Asset* _asset) { Item_Info = _asset; }
	void Set_IsGetType(bool* _get, bool* _get2) 
	{ 
		bGetType = _get; 
		bGetItem = _get2; 
	}

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:
	void OnTextButtonClicked();
	UFUNCTION()
	void OnSelectButton();

	bool Widget_Setting();

protected  :
	/* 아이템 이미지*/
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Item_Image;
	/* 텍스트 버튼 */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Text_Button;
	/* 이름 텍스트*/
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> ItemName_Text;
	/* 가격표 */
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Price_Text;

private :
	TObjectPtr<class UVerticalBox> Verticle_Canvas;
	FItemData_Asset*		Item_Info;

	FSlateColor				InitTextColor;

	bool						bHovering = { false };
	bool*						bGetType = { nullptr };

	bool*						bGetItem = { nullptr };
};
