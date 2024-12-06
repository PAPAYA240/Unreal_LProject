

#include "UI/InGame/Store/RestArea_Widget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"

URestArea_Widget::URestArea_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URestArea_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* Service_Button = Cast<UButton>(GetWidgetFromName(TEXT("Service_Button"))))
		Service_Button->OnClicked.AddDynamic(this, &URestArea_Widget::OnServiceButtonClick);

	if (UButton* Quest_Button = Cast<UButton>(GetWidgetFromName(TEXT("Quest_Button"))))
		Quest_Button->OnClicked.AddDynamic(this, &URestArea_Widget::OnQuestButtonClick);

	if (UButton* Store_Button = Cast<UButton>(GetWidgetFromName(TEXT("Store_Button"))))
		Store_Button->OnClicked.AddDynamic(this, &URestArea_Widget::OnStoreButtonClick);

	if (UButton* Exit_Button = Cast<UButton>(GetWidgetFromName(TEXT("Exit_Button"))))
		Exit_Button->OnClicked.AddDynamic(this, &URestArea_Widget::OnExitButtonClick);

	if (UButton* QuestExit_Button = Cast<UButton>(GetWidgetFromName(TEXT("QuestExit_Button"))))
		QuestExit_Button->OnClicked.AddDynamic(this, &URestArea_Widget::OnQuestExitButtonClick);

	if (UButton* StoreExit_Button = Cast<UButton>(GetWidgetFromName(TEXT("StoreExit_Button"))))
		StoreExit_Button->OnClicked.AddDynamic(this, &URestArea_Widget::OnStoreExitButtonClick);
}

void URestArea_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void URestArea_Widget::OnServiceButtonClick()
{
	Select_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Service_Canvas->SetVisibility(ESlateVisibility::Visible);
}

void URestArea_Widget::OnQuestButtonClick()
{
	Select_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Quest_Canvas->SetVisibility(ESlateVisibility::Visible);
}

void URestArea_Widget::OnStoreButtonClick()
{
	Select_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Store_Canvas->SetVisibility(ESlateVisibility::Visible);
}

void URestArea_Widget::OnExitButtonClick()
{
	Set_ShowWidget(false);

	for (auto iter : Canvas->GetAllChildren())
	{
		UCanvasPanel* uCanvas = Cast<UCanvasPanel>(iter);
		if (nullptr != uCanvas)
			uCanvas->SetVisibility(ESlateVisibility::Hidden);
	}
	Select_Canvas->SetVisibility(ESlateVisibility::Visible);
	bExit = true;
}

void URestArea_Widget::OnQuestExitButtonClick()
{
	Select_Canvas->SetVisibility(ESlateVisibility::Visible);
	Quest_Canvas->SetVisibility(ESlateVisibility::Hidden);
}

void URestArea_Widget::OnStoreExitButtonClick()
{
	Select_Canvas->SetVisibility(ESlateVisibility::Visible);
	Store_Canvas->SetVisibility(ESlateVisibility::Hidden);
}
