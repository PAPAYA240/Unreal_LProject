
#include "UI/Lobby/AbilityInfo_Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

UAbilityInfo_Widget::UAbilityInfo_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAbilityInfo_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	HoveringInfo.abilityType = ABILITY_TYPE::END;
}

void UAbilityInfo_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Chang_Info();
}

void UAbilityInfo_Widget::Chang_Info()
{
	if (Canvas)
	{
		if (true == HoveringInfo.bHovering)
		{
			Canvas->SetVisibility(ESlateVisibility::Visible);

			SetCanvasPosition();
		}
		else if (false == HoveringInfo.bHovering)
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UAbilityInfo_Widget::SetCanvasPosition()
{
	FText Name, Info;

	switch (HoveringInfo.abilityType)
	{
	case ABILITY_TYPE::POWER:
		Name = FText::FromString(TEXT("힘"));
		Info = FText::FromString(TEXT("캐릭터의 물리적 힘을 나타내는 능력치입니다. \n"
			"높을수록 근접 전투에서의 공격력이 증가하며, \n"
			"물체를 밀거나 던지는 능력에도 영향을 미쳐, \n"
			"다양한 상황에서 유용하게 사용될 수 있습니다."));
		break;

	case ABILITY_TYPE::DEFENCE:
		Name = FText::FromString(TEXT("방어"));
		Info = FText::FromString(TEXT("알 수 없음"));
		break;

	case ABILITY_TYPE::SPEED:
		Name = FText::FromString(TEXT("속도"));
		Info = FText::FromString(TEXT("알 수 없음"));

		break;
	case ABILITY_TYPE::INTELLECTUAL:
		Name = FText::FromString(TEXT("지력"));
		Info = FText::FromString(TEXT("알 수 없음"));
		break;

	case ABILITY_TYPE::HILL:
		Name = FText::FromString(TEXT("치유"));
		Info = FText::FromString(TEXT("알 수 없음"));
		break;

	case ABILITY_TYPE::LUCKY:
		Name = FText::FromString(TEXT("행운"));
		Info = FText::FromString(TEXT("알 수 없음"));
		break;

	default:
		Name = FText::FromString(TEXT("알 수 없음"));
		Info = FText::FromString(TEXT("알 수 없음"));
		break;
	}

	AbilityName_Text->SetText(Name);
	AbilityInfo_Text->SetText(Info);

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Canvas->Slot);
	if (CanvasSlot)
		CanvasSlot->SetPosition(HoveringInfo.HoveringPos); // 새로운 위치 설정
}