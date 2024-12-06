

#include "UI/InGame/Store/R1Merchant_ItemInfo.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"

#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UR1Merchant_ItemInfo::UR1Merchant_ItemInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR1Merchant_ItemInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Widget_Setting())
		MSG_BOX(TEXT("UR1Merchant_ItemInfo() : 위젯이 제대로 세팅되지 않았습니다."));

}

void UR1Merchant_ItemInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(Verticle_Canvas)
	{
		if (ESlateVisibility::Visible == Verticle_Canvas->GetVisibility())
		{
			Set_ShowWidget(true);
			OnTextButtonClicked();

			// 이전 UI선택 아이템에 텍스처만 있었을 때 색 바꾸기
			/*if (Text_Button->IsHovered())
			{
				if (ItemName_Text)
				{
					FLinearColor NewColor = FLinearColor::Gray;
					ItemName_Text->SetColorAndOpacity(NewColor);
				}
				bHovering = true;
			}
			else
			{
				if(ItemName_Text)
					ItemName_Text->SetColorAndOpacity(InitTextColor);
				bHovering = false;
			}*/
		}
		else
			Set_ShowWidget(false);
	}
}

void UR1Merchant_ItemInfo::OnTextButtonClicked()
{
	/*if (!bGetType || !bGetItem)
		return;*/

	if(bHovering)
	{
		if (CurrentPlayerState->TotalCoins >= Item_Info->ItemPurchasePrice)
		{
			CurrentPlayerState->TotalCoins -= Item_Info->ItemPurchasePrice;

			// 1. 플레이어 인벤토리로 넘기기
			(*bGetType) = true;
			(*bGetItem) = true;

			R1Player->Add_InventoryItem(Item_Info);
		}
		else
		{
			(*bGetType) = true;
			(*bGetItem) = false;
		}
		bHovering = false;
	}
}

void UR1Merchant_ItemInfo::OnSelectButton()
{
	bHovering = true;
}

bool UR1Merchant_ItemInfo::Widget_Setting()
{
	InitTextColor = ItemName_Text->GetColorAndOpacity();

	// Image
	if (Item_Info)
	{
		if (Item_Info->ItemTexture)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(Item_Info->ItemTexture);
			Item_Image->SetBrush(Brush);
		}
		// Name 
		ItemName_Text->SetText(FText::FromString(Item_Info->ItemName));
		// 가격 설정
		Price_Text->SetText(FText::FromString(FString::Printf(TEXT("%d"), Item_Info->ItemPurchasePrice)));
	}
	else
		return false;

	if (UButton* ItemButton = Cast<UButton>(GetWidgetFromName(TEXT("Item_Button"))))
		ItemButton->OnClicked.AddDynamic(this, &UR1Merchant_ItemInfo::OnSelectButton);
	else
		return false;
	return true;
}
