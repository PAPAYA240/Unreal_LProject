

#include "UI/InGame/BuildingEvent_Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Player/R1Player.h"

UBuildingEvent_Widget::UBuildingEvent_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UBuildingEvent_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* PrayerButton = Cast<UButton>(GetWidgetFromName(TEXT("Prayer_Button"))))
		PrayerButton->OnClicked.AddDynamic(this, &UBuildingEvent_Widget::OnPrayClicked);

	if (UButton* ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("Exit_Button"))))
		ExitButton->OnClicked.AddDynamic(this, &UBuildingEvent_Widget::OnExitClicked);
}

void UBuildingEvent_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UBuildingEvent_Widget::OnPrayClicked()
{
	// 악마일 때 HP -50 감소
	if (EBUILDING_TYPE::DEVIL_EVENT == Building_Type)
		R1Player->OnDamged(50, nullptr);

	bPlayRandom = true;
}

void UBuildingEvent_Widget::OnExitClicked()
{
	Set_Exit(true); // 위젯 나가기
	Set_ShowWidget(false);
}

void UBuildingEvent_Widget::Setting_WidgetText(EBUILDING_TYPE _building)
{
	switch (_building)
	{
	case EBUILDING_TYPE::DEVIL_EVENT:
	{
		if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Name_Text"))))
			NameText->SetText(FText::FromString(TEXT("대가의 악마")));
		if (UTextBlock* MainDocumText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MainDocum_Text"))))
			MainDocumText->SetText(FText::FromString(TEXT("그대여, 나와 거래 하나 하지 않겠는가?")));
		if (UTextBlock* SubDocumText = Cast<UTextBlock>(GetWidgetFromName(TEXT("SubDocum_Text"))))
			SubDocumText->SetText(FText::FromString(TEXT("악마와 거래하여 스킬을 획득하시겠습니까?")));
		if (UTextBlock* RightButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RightButton_Text"))))
			RightButtonText->SetText(FText::FromString(TEXT("거래하기")));
		if (UTextBlock* ReferenceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Reference_Text"))))
			ReferenceText->SetText(FText::FromString(TEXT("거래의 대가로 HP[-50] 을 가져가기로 합니다.")));
		Building_Type = EBUILDING_TYPE::DEVIL_EVENT;
	}
		break;
	default :
		break;
	}
}
