// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Inventory/PlayerInventory_Widget.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "UI/InGame/Store/R1Merchant_ItemInfo.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

#include "R1GameModeBase.h"
#include "Kismet/GameplayStatics.h"

UPlayerInventory_Widget::UPlayerInventory_Widget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UPlayerInventory_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_WidgetOption())
		MSG_BOX(TEXT("UPlayerInventory_Widget() : Widget Option이 올바르지 않습니다."));
}

void UPlayerInventory_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	R1Player = GameMode->CurrentPlayer;
	if (false == Owner_Player->Get_OpenInventory() || Owner_Player != R1Player)
		return;

	Super::NativeTick(MyGeometry, InDeltaTime);

	if(Inventory_Gold != Owner_Player->Get_CurrentPlayerState()->TotalCoins)
	{
		Inventory_Gold = Owner_Player->Get_CurrentPlayerState()->TotalCoins;
		Gold_Text->SetText(FText::AsNumber(Inventory_Gold));
	}

	Open_Inventory();
}

void UPlayerInventory_Widget::OnExitClicked()
{
	Set_ShowWidget(false);
	R1Player->Set_OpenInventory(false);
}

void UPlayerInventory_Widget::OnButtonClick()
{
	int32 iItemCnt = { 0 };
	for (auto iter : InvenGrid_Canvas->GetAllChildren())
	{
		if (UButton* button = Cast<UButton>(iter))
		{
			if (true == button->IsHovered())
			{
				if (!(CurrentPlayerState = Owner_Player->Get_CurrentPlayerState()))
					return;

				if(iItemCnt <= CurrentPlayerState->ItemAssets.Num())
				{
					if (nullptr == CurrentPlayerState->ItemAssets[iItemCnt])
						return;

					/* 0. Item 사용 시 Player의 영향 : Item을 끄집에내서 사용해야겠다 */
					R1Player->Using_Item(CurrentPlayerState->ItemAssets[iItemCnt]->ItemType);
					/* 1. Player의 Inventory 배열 비우기*/
					CurrentPlayerState->ItemAssets[iItemCnt] = nullptr;

					/* 2. 없어진 아이템의 Inventory 창 비우기*/
					for (auto iter2 : button->GetAllChildren())
					{
						if (UImage* _itemImage = Cast<UImage>(iter2))
						{
							_itemImage->SetVisibility(ESlateVisibility::Hidden);
							return;
						}
					}
				}
			}
			else
				++iItemCnt;
		}
	}
}

void UPlayerInventory_Widget::Open_Inventory()
{
	if(ESlateVisibility::Hidden == Get_ShowWidget())
		Set_ShowWidget(true);
}

void UPlayerInventory_Widget::AddWidgetToVerticalBox(FItemData_Asset* _asset)
{
	if (InvenGrid_Canvas)
	{
		// 1. 인벤토리 빈 박스 찾기
		UImage* itemImage = Find_VoidBox();

		// 2. 인벤토리 아이템 할당
		if (itemImage)
		{
			if (!(CurrentPlayerState = Owner_Player->Get_CurrentPlayerState()))
				return;

			itemImage->SetVisibility(ESlateVisibility::Visible);
			ChangeImage(itemImage, _asset->ItemTexture);
			FVector2D vSize = FVector2D{ 70.f, 70.f };
			SetImageSize_Outward(itemImage, vSize);
			CurrentPlayerState->ItemAssets.Emplace(_asset);
		}
		else
		{
			MSG_BOX(TEXT("아이템 || 인벤토리에 공간이 없습니다."));
		}
		//if (MerchantItem_Instance)
			//{
			//	MerchantItem_Instance->AddToViewport();
			//	MerchantItem_Instance->Set_VerticalPtr(Vertical_Canvas);
			//	MerchantItem_Instance->Set_ItemInfo(_asset);
			//	// VerticalBox에 추가
			//	UVerticalBoxSlot* _Slot = Vertical_Canvas->AddChildToVerticalBox(MerchantItem_Instance);
			//	if (_Slot)
			//		_Slot->SetPadding(FMargin(5.0f));
			//	// Player에게 아이템 정보 전달하기
			//}
	}
}

bool UPlayerInventory_Widget::Setting_WidgetOption()
{
	// 0. Button 등록
	if (UButton* ButtonExit = Cast<UButton>(GetWidgetFromName(TEXT("Button_Exit"))))
		ButtonExit->OnClicked.AddDynamic(this, &UPlayerInventory_Widget::OnExitClicked);
	else
		return false;
	
	// 1. 인벤토리 아이템 이미지 Render 끄기
	if (InvenGrid_Canvas)
	{
		for (auto iter : InvenGrid_Canvas->GetAllChildren())
		{
			if(UButton* button = Cast<UButton>(iter))
			{
				button->OnClicked.AddDynamic(this, &UPlayerInventory_Widget::OnButtonClick);

				for (auto iter2 : button->GetAllChildren())
				{
					if (UImage* _image = Cast<UImage>(iter2))
					{
						_image->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		}
	}
	else
		return false;
	return true;
}

UImage* UPlayerInventory_Widget::Find_VoidBox()
{
	for(auto iter : InvenGrid_Canvas->GetAllChildren())
	{
		if (UButton* button = Cast<UButton>(iter))
		{
			for (auto iter2 : button->GetAllChildren())
			{
				UImage* _image = Cast<UImage>(iter2);
				if (nullptr != _image && ESlateVisibility::Hidden == _image->GetVisibility())
				{
					return _image;
				}
			}
		}
	}
	return nullptr;
}

