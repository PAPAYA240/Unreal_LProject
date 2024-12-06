
#pragma once

#include "CoreMinimal.h"
#include "Item/R1ItemData.h"
#include "UI/R1UserWidget.h"
#include "Store_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UStore_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UStore_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void					Set_PlayerContect(bool _contect) { bPlayerContect = _contect;  }
	// 상점에 아이템 추가
	void					AddWidgetToVerticalBox(FItemData_Asset* _asset);

	void					ShowStore_Widget(ESlateVisibility _visib);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnExit_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnDialog_Clicked();
	
	UFUNCTION(BlueprintCallable)
	void OnStore_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnCooperation_Clicked();

	void			Reset();

private :
	bool			Player_Context();
	bool			Open_Store();
	bool			GetType_Operate();


protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> MerchantItem_Widget;

protected: 
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Contect_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Store_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Dialog_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> CoinCnt;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UVerticalBox> Vertical_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> GetType_Text;
	
private :
	bool bPlayerContect = { false };
	bool bDialogEnd = { false };
	float PauseStoreOpen_Timer = { 0.0f };

	bool bGetType = { false };
	bool bGetItem = { false };

	bool bGetType_Setting = { false };
	float GetType_Timer = { 0.0f };
	float OpercityValue = { 1.f };
	FVector2D GetTypeText_InitPos;
};
