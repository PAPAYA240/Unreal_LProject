

#include "UI/InGame/RandomBox_Widget.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Item/R1ItemData.h"

URandomBox_Widget::URandomBox_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URandomBox_Widget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!WidgetOption_Setting())
		MSG_BOX(TEXT("URandomBox_Widget() : Widget Option이 세팅되지 않았습니다."));
	
}

void URandomBox_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (true == bStart)
	{
		Opreate_RandomBox();
	}
}

void URandomBox_Widget::ClickStartOption()
{
	bStart = true;
}

bool URandomBox_Widget::WidgetOption_Setting()
{
	if (UCanvasPanel* ItemCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Item_Canvas"))))
	{
		for (auto iter : ItemCanvas->GetAllChildren())
		{
			UImage* _image = Cast<UImage>(iter);

			if (_image)
				ItemImage_Array.Emplace(_image);
		}
	}
	else
		return false;

	if (ItemImage_Array.Num() <= 0)
		return false;

	if (UButton* StartButton = Cast<UButton>(GetWidgetFromName(TEXT("Start_Button"))))
	{
		StartButton->OnClicked.AddDynamic(this, &URandomBox_Widget::ClickStartOption);
	}
	else
		return false;

	return true;
}

void URandomBox_Widget::Opreate_RandomBox()
{
	RandomBox_Timer += GetWorld()->GetDeltaSeconds();
	FLinearColor GrayColor(0.5f, 0.5f, 0.5f, 1.0f);
	FLinearColor OriginColor(1.f, 1.f, 1.f, 1.0f);

	if (RandomBox_Timer <= 6.f)
	{
		if (RandomBox_Timer >= 2.f)
		{
			RandomSpeed += GetWorld()->GetDeltaSeconds() * 4.f;
			CurrentRandomSpeed += GetWorld()->GetDeltaSeconds();
		}

		if (SelectItemCnt < ItemImage_Array.Num())
		{
			ItemImage_Array[SelectItemCnt]->SetBrushTintColor(GrayColor);

			if (0 <= SelectItemCnt - 1 && SelectItemCnt - 1 <= ItemImage_Array.Num())
				ItemImage_Array[SelectItemCnt - 1]->SetBrushTintColor(OriginColor);
		}
		else
		{
			SelectItemCnt = 0;
			ItemImage_Array[SelectItemCnt]->SetBrushTintColor(GrayColor);
			ItemImage_Array[ItemImage_Array.Num() - 1]->SetBrushTintColor(OriginColor);
		}

		if (CurrentRandomSpeed <= RandomSpeed || RandomBox_Timer < 2.f)
		{
			RandomSpeed = 0.f;
			++SelectItemCnt;
		}
	}
	// 정답! 깜빡 거리기
	else if (RandomBox_Timer <= 9.f)
	{

		SelectItem_Timer += GetWorld()->GetDeltaSeconds();
		if (0 <= SelectItemCnt - 1 && SelectItemCnt - 1 <= ItemImage_Array.Num())
			ItemImage_Array[SelectItemCnt - 1]->SetBrushTintColor(OriginColor);
		else
			ItemImage_Array[ItemImage_Array.Num() - 1]->SetBrushTintColor(OriginColor);

		if (SelectItem_Timer >= 0.3f)
		{
			if (false == bSelectColor)
				ItemImage_Array[SelectItemCnt]->SetBrushTintColor(GrayColor);

			else if (true == bSelectColor)
				ItemImage_Array[SelectItemCnt]->SetBrushTintColor(OriginColor);
			Reward_Type = static_cast<ERANDOMBOX_TYPE>(SelectItemCnt);
			SelectItem_Timer = 0.f;
			bSelectColor = !bSelectColor;
		}
	}
	else // 아이템 보상 제공
	{
		Set_ShowWidget(false);
		if (true == Select_Reward())
		{
			bExit = true;
			bStart = false;
		}
	}
}

bool URandomBox_Widget::Select_Reward()
{
	Set_ShowWidget(false);
	switch (Reward_Type)
	{
	case ERANDOMBOX_TYPE::FULL_ENERGY: // 2. 모든 에너지 채우기
	{
		MSG_BOX(TEXT("에너지 꽉 채움"));
		R1Player->Add_PlayerEnergy(3);
		return true;

	}
	break;

	case ERANDOMBOX_TYPE::GET_ATTACKPOTION: // 공격 물약 획득 보상
	{
		MSG_BOX(TEXT("공격 물약 획득"));
		return true;
	}
	break;
	case ERANDOMBOX_TYPE::FULL_HP :		// 1. Max 보상
	{
		MSG_BOX(TEXT("hp 꽉채움"));
		if (R1Player->Update_MaxHp(20))
			return true;
	}
	break;

	case ERANDOMBOX_TYPE::ADD1_ENERGY: // 에너지 한 개 보상
	{
		MSG_BOX(TEXT("에너지 1개 보상"));
		R1Player->Add_PlayerEnergy(1);
		return true;
	}
	break;

	case ERANDOMBOX_TYPE::MINUS15_ATTACK: // - 15 Attack 획득
	{
		MSG_BOX(TEXT("어택 -15"));
		R1Player->Update_MaxAttack(-15);
		return true;
	}
	break;
	
	case ERANDOMBOX_TYPE::ADD500_COIN : // 500 Coin 획득
	{
		MSG_BOX(TEXT("코인 500개"));
		R1Player->Get_CurrentPlayerState()->TotalCoins += 500;
		return true;
	}
	break;

	case ERANDOMBOX_TYPE::GET_TELEPORT_SCROLL: // Scroll 
	{
		MSG_BOX(TEXT("텔레포트 스크롤 획득"));

		return true;

	}
	break;

	case ERANDOMBOX_TYPE::GET_ONE_DEAD :
	{
		MSG_BOX(TEXT("죽음 1개 추가"));
		++R1Player->Get_CurrentPlayerState()->Current_DeadLine;
		return true;
	}
	break;

	case ERANDOMBOX_TYPE::MINUS100_COIN:
	{
		MSG_BOX(TEXT("코인 1개 뺏김"));
		R1Player->Get_CurrentPlayerState()->TotalCoins -= 100;

		if (R1Player->Get_CurrentPlayerState()->TotalCoins < 0)
			MSG_BOX(TEXT("빚이 생겼습니다. 이제부터 상점 서비스가 제한됩니다."));
		return true;
	}
	break;

	case ERANDOMBOX_TYPE::MINUS25_HP :
	{
		MSG_BOX(TEXT("hp - 25"));
		if (R1Player->Update_MaxHp(-25))
			return true;
	}
	break;

	case ERANDOMBOX_TYPE::ADD20_HP : // MaxHp 20up 보상
	{
		MSG_BOX(TEXT("hp + 20"));
		R1Player->DecreaseHP(R1Player->CurrentHp, R1Player->MaxHp, GetWorld()->GetDeltaSeconds());
		if (FMath::IsNearlyEqual(R1Player->CurrentHp, R1Player->MaxHp, 0.01f))
		{
			R1Player->Hp = R1Player->CurrentHp = R1Player->MaxHp;
			return true;
		}
	}
	break;
	}

	return false;
}

