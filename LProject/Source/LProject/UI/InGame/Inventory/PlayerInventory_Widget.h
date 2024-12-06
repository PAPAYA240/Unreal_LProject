// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "Item/R1ItemData.h"
#include "PlayerInventory_Widget.generated.h"

UCLASS()
class LPROJECT_API UPlayerInventory_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UPlayerInventory_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void AddWidgetToVerticalBox(FItemData_Asset* _asset);

protected :
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public :	  // For. Getter Setter 
	void Set_OwnerPlayer(class AR1Player* _player) { Owner_Player = _player; }

protected : // For. Delegate
	UFUNCTION(BlueprintCallable)
	void		OnExitClicked();
	UFUNCTION(BlueprintCallable)
	void		OnButtonClick();

private :
	// For. Setting
	bool					Setting_WidgetOption();
	// 인벤토리 열기
	void					Open_Inventory();
	// 빈 박스를 탐색
	class UImage*		Find_VoidBox(); 

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UUniformGridPanel> InvenGrid_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Gold_Text;

	UPROPERTY()
	TObjectPtr<class AR1Player> Owner_Player; // Widget의 Player주인

	int32			Inventory_Gold = { 0 };
	
};
