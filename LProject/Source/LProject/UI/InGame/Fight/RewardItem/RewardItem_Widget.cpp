// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Fight/RewardItem/RewardItem_Widget.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"
#include "Monster/R1Monster.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

URewardItem_Widget::URewardItem_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URewardItem_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Get_Button)
		Get_Button->OnClicked.AddDynamic(this, &URewardItem_Widget::OnGet_Clicked);

	if(GetOut_Button)
		GetOut_Button->OnClicked.AddDynamic(this, &URewardItem_Widget::OnGetOut_Clicked);

	if (Canvas)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Canvas->Slot);
		if (CanvasSlot)
		{
			FVector2D CurrentPosition = CanvasSlot->GetPosition();
			Canvas_initPos = CurrentPosition;
		}
	}

	NextBG_initPos = GetImagePosition(Next_BackGround);
	Eatable_ItemCount = 2; // 먹을 수 있는 아이템 (or Monster가 뱉어낼 수 있는 아이템 개수)
	Set_ShowWidget(false);
}

void URewardItem_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Find_ItemType(); // 먹을 아이템 찾기
	if (Operate_GetIemWidget()) // 아이템을 먹을 때 Widget 작동
		Operate_NextBackground(); // 다음 Scene으로 넘어가기

}

void URewardItem_Widget::OnGet_Clicked()
{
	bOperateWidget = false;

	if (Item_Asset)
		R1Player->Add_InventoryItem(Item_Asset);
}

void URewardItem_Widget::OnGetOut_Clicked()
{
}

bool URewardItem_Widget::Operate_RewardWidget()
{
	Set_ShowWidget(true);

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Canvas->Slot);
	if (CanvasSlot)
	{
		FVector2D CurrentPosition = CanvasSlot->GetPosition();
		Canvas_initPos = CurrentPosition;
		FVector2D TargetPosition = Canvas_initPos + FVector2D(100.f, 0.f); 
		CanvasSlot->SetPosition(TargetPosition);
	}
	return false;
}

void URewardItem_Widget::Find_ItemType()
{
	// 먹을 수 있는 아이템의 한도가 지나버렸다면 
	if (CurrentEatable_ItemCount > Eatable_ItemCount)
	{
		EatableItem_End = true;
		return;
	}

	if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
		if (true == R1Player->Get_EndFight())
		{
			if (false == bOperateWidget)
			{
				if (CurrentPlayerState->TargetMonster)
				{
					bOperateWidget = true;
					Item_Asset = CurrentPlayerState->TargetMonster->Get_RewardItem();

					if (nullptr == Item_Asset)
					{
						bOperateWidget = false;
						EatableItem_End = true;
						return;
					}
					else
					{
						// 아이템 이름
						ItemName_Text->SetText(FText::FromString(Item_Asset->ItemName));

						// 아이템 사진
						FSlateBrush Brush;
						Brush.SetResourceObject(Item_Asset->ItemTexture);
						Item_Image->SetBrush(Brush);

						++CurrentEatable_ItemCount;
					}
				}
			}
		}
	}

}

bool URewardItem_Widget::Operate_GetIemWidget()
{
	if (EatableItem_End)
		return true;

	if (true == bOperateWidget)
		Operate_RewardWidget();
	else
		Set_ShowWidget(false);

	return false;
}

void URewardItem_Widget::Operate_NextBackground()
{
	FVector2D CurrentPos = GetImagePosition(Next_BackGround);

	FVector2D TargetPos = FVector2D{ NextBG_initPos.X - (GetWindowSize().X * 1.8f), CurrentPos.Y };
	
	FVector2D NewPos = VInterpTo2D(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 5.f);

	MoveImage(Next_BackGround, NewPos);

	if (10.f >= FVector2D::Distance(NewPos, TargetPos))
	{
		R1Player->Change_Level(ESCENE::INGAME);
	}
}
