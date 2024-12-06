

#include "UI/InGame/HintDialog_Widget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UHintDialog_Widget::UHintDialog_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHintDialog_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* LetGOButton = Cast<UButton>(GetWidgetFromName(TEXT("LetGO_Button"))))
		LetGOButton->OnClicked.AddDynamic(this, &UHintDialog_Widget::OnLetGo_Clicked);
}

void UHintDialog_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UHintDialog_Widget::OnLetGo_Clicked()
{
	Set_ShowWidget(false);
	bExit = true;
}
