

#include "UI/InGame/Ability/HerblistAbility_Widget.h"
#include "Player/Player_Herblist/R1Player_Herbalist.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UHerblistAbility_Widget::UHerblistAbility_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHerblistAbility_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_WidgetOption())
		MSG_BOX(TEXT("UHerblistAbility_Widget() : Widget Option이 세팅되지 않았습니다."));
}

void UHerblistAbility_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OwnerPlayer)
	{
		// 약초를 찾았다면
		if (true == OwnerPlayer->Get_FindHerb())
		{
			if (ESlateVisibility::Hidden == Get_ShowWidget())
			{
				Set_ShowWidget(true);
			}
		}
	}

	if (true == bFind)
	{
		Find_Hurb();
		Kind_Hurb();
	}
	if (true == bEnd)
	{
		if (ESlateVisibility::Visible == Get_ShowWidget())
		{
			FindHurb_Timer += GetWorld()->GetDeltaSeconds();
			if(FindHurb_Timer >= 0.8f)
			{
				bEnd = false;
				FindHurb_Timer = 0.f;
				Set_ShowWidget(false);
				OwnerPlayer->Set_FindHerb(false);
			}
		}
	}
}

void UHerblistAbility_Widget::OnFindButtonClicked()
{
	bFind = true;
}

bool UHerblistAbility_Widget::Setting_WidgetOption()
{
	Set_ShowWidget(false);

	if (UButton* FindButton = Cast<UButton>(GetWidgetFromName(TEXT("Find_Button"))))
		FindButton->OnClicked.AddDynamic(this, &UHerblistAbility_Widget::OnFindButtonClicked);
	else
		return false;
	return true;
}

void UHerblistAbility_Widget::Find_Hurb()
{
	FindHurb_Timer += GetWorld()->GetDeltaSeconds();
	if (FindHurb_Timer <= 2.f)
	{
		int32 iHurbType = static_cast<int32>(HurbType) + 1;
		HurbType = static_cast<EHURB_TYPE>(iHurbType);
		if (HurbType >= EHURB_TYPE::END_HERB)
			HurbType = EHURB_TYPE::DONE_HRUB;
	}
	else 
	{
		FindHurb_Timer = 0.f;
		bFind = false;
		bEnd = true;
	}

}

void UHerblistAbility_Widget::Kind_Hurb()
{
	switch (HurbType)
	{
	case EHURB_TYPE::DONE_HRUB:
	{
		Docum_Text->SetText(FText::FromString(TEXT("잡초를 발견했다!")));
	}
	break;

	case EHURB_TYPE::DEFAULT_HURB:
	{
		Docum_Text->SetText(FText::FromString(TEXT("기본 약초를 발견했다!")));
	}
	break;

	case EHURB_TYPE::GOOD_HERB:
	{
		Docum_Text->SetText(FText::FromString(TEXT("좋은 약초를 발견했다!")));
	}
	break;

	case EHURB_TYPE::SUPER_HERB:
	{
		Docum_Text->SetText(FText::FromString(TEXT("슈퍼 약초를 발견했다!")));
	}
	break;

	default:
	{
		Docum_Text->SetText(FText::FromString(TEXT("에러를 발견했다!")));
	}
	break;
	}
}
