

#include "UI/InGame/Store/Store_Widget.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"
#include "UI/InGame/Store/R1Merchant_ItemInfo.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Image.h"

UStore_Widget::UStore_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UStore_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	/* 나가기 버튼 */
	if (UButton* ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("Exit_Button"))))
		ExitButton->OnClicked.AddDynamic(this, &UStore_Widget::OnExit_Clicked);

	/* 대화 시작 시*/
	if (UButton* ContectButton = Cast<UButton>(GetWidgetFromName(TEXT("Contect_Button"))))
		ContectButton->OnClicked.AddDynamic(this, &UStore_Widget::OnDialog_Clicked);

	/* 상점 클릭 시 */	
	if (UButton* StoreButton = Cast<UButton>(GetWidgetFromName(TEXT("Store_Button"))))
		StoreButton->OnClicked.AddDynamic(this, &UStore_Widget::OnStore_Clicked);

	/* 협력 버튼 클릭*/
	if (UButton* CooperationButton = Cast<UButton>(GetWidgetFromName(TEXT("Cooperation_Button"))))
		CooperationButton->OnClicked.AddDynamic(this, &UStore_Widget::OnCooperation_Clicked);

	Reset();
	GetTypeText_InitPos = GetTextPosition(GetType_Text);
}

void UStore_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		if (!Player_Context())
			Reset();
		else
		{
			/* 대화 시작 전 */
			if (false == bDialogEnd)
			{
				Canvas->SetVisibility(ESlateVisibility::Visible);
				Contect_Canvas->SetVisibility(ESlateVisibility::Visible);
			}

			/* Inventory 동작*/
			GetType_Operate();
		}
	}
}

void UStore_Widget::OnExit_Clicked()
{
	Reset();
	bExit = true;
}

void UStore_Widget::OnDialog_Clicked()
{
	R1Player->End_Turn(false);
	Contect_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Dialog_Canvas->SetVisibility(ESlateVisibility::Visible);
	bDialogEnd = true;
}

void UStore_Widget::OnStore_Clicked()
{
	Dialog_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Store_Canvas->SetVisibility(ESlateVisibility::Visible);
	Vertical_Canvas->SetVisibility(ESlateVisibility::Visible);
}


void UStore_Widget::OnCooperation_Clicked()
{
}

void UStore_Widget::Reset()
{
	if (Canvas)
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
		Contect_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Store_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Dialog_Canvas->SetVisibility(ESlateVisibility::Hidden);

		bDialogEnd = false;
	}
}

bool UStore_Widget::Player_Context()
{
	return (true == bPlayerContect && !CurrentPlayerState->TargetMonster) ? true : false;
}

bool UStore_Widget::Open_Store()
{
	if (false == bExit)
		return true;

	else if (true == bExit)
	{
		PauseStoreOpen_Timer += GetWorld()->GetDeltaSeconds();

		if (PauseStoreOpen_Timer >= 2.f)
		{
			bExit = false;
			PauseStoreOpen_Timer = 0.f;

			return true;
		}
		return false;
	}
	return false;
}

// 아이템 선택 시 돈 상태에 따라 아이템 GEt 상태 확인
bool UStore_Widget::GetType_Operate()
{
	if (false == bGetType)
		return false;

	if (CoinCnt)
		CoinCnt->SetText(FText::Format(NSLOCTEXT("YourNamespace", "CoinCountFormat", "{0}"), FText::AsNumber(CurrentPlayerState->TotalCoins)));

	if(false == bGetType_Setting)
	{
		bGetType_Setting = true;

		if (true == bGetType)
			GetType_Text->SetOpacity(1.f);

		else if (false == bGetType)
			GetType_Text->SetOpacity(0.f);

		if(true == bGetItem)
			GetType_Text->SetText(FText::FromString(TEXT("획득")));
		else
			GetType_Text->SetText(FText::FromString(TEXT("잔액 부족")));
	}

	else
	{
		GetType_Timer += GetWorld()->GetDeltaSeconds();
		OpercityValue -= GetWorld()->GetDeltaSeconds();

		GetType_Text->SetOpacity(OpercityValue);

		if(GetType_Timer >= 0.1f)
		{
			// 텍스트 이동
			FVector2D TargetPos = GetTypeText_InitPos;
			TargetPos.X -= 300.f;
			FVector2D CurrentPos = GetTextPosition(GetType_Text);
			FVector2D NewPos = VInterpTo2D(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 4.f);
			MoveText(GetType_Text, NewPos);

			if (OpercityValue <= 0.f)
			{
				MoveText(GetType_Text, GetTypeText_InitPos);
				GetType_Text->SetOpacity(0.f);

				bGetType_Setting = false;
				bGetType = false;

				GetType_Timer = 0.f;
				OpercityValue = 1.f;
				return true;
			}
		}
	}
	return false;
}

void UStore_Widget::AddWidgetToVerticalBox(FItemData_Asset* _asset)
{
	if (Vertical_Canvas)
	{
		if (MerchantItem_Widget)
		{
			UR1Merchant_ItemInfo* MerchantItem_Instance = Cast<UR1Merchant_ItemInfo>(CreateWidget(GetWorld(), MerchantItem_Widget));
			if (MerchantItem_Instance)
			{
				MerchantItem_Instance->AddToViewport();
				MerchantItem_Instance->Set_VerticalPtr(Vertical_Canvas);
				MerchantItem_Instance->Set_ItemInfo(_asset);
				MerchantItem_Instance->Set_IsGetType(&bGetType, &bGetItem);

				// VerticalBox에 추가
				UVerticalBoxSlot* _Slot = Vertical_Canvas->AddChildToVerticalBox(MerchantItem_Instance);
				if (_Slot)
					_Slot->SetPadding(FMargin(5.0f));
			}
			else
				MSG_BOX(TEXT("위젯 생성 실패: MerchantItem_Instance가 nullptr입니다."));
		}
	}
}

void UStore_Widget::ShowStore_Widget(ESlateVisibility _visib)
{
	if(Store_Canvas)
	{
		Canvas->SetVisibility(_visib);
		Store_Canvas->SetVisibility(_visib);
	}
}
