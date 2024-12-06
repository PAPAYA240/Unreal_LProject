

#include "UI/R1UserWidget.h"
#include "R1UserWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"     

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"
#include "Math/UnrealMathUtility.h" // FMath 헤더 포함

UR1UserWidget::UR1UserWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UR1UserWidget::NativeConstruct()
{
    Super::NativeConstruct();

	if (true == bParentTick)
	{
		if (false == Find_Player())
			MSG_BOX(TEXT("플레이어가 존재하지 않음 : Widget"));
	}
}

void UR1UserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

	if (false == Find_Player())
		MSG_BOX(TEXT("플레이어가 존재하지 않음 : Widget"));
}

bool UR1UserWidget::Find_Player()
{
	if (R1Player)
	{
		CurrentPlayerState = R1Player->Get_CurrentPlayerState();
		return true;
	}

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if(GameMode)
	{
		R1Player = GameMode->CurrentPlayer;
		if(R1Player)
			CurrentPlayerState = R1Player->Get_CurrentPlayerState();
	}
   return (R1Player != nullptr && CurrentPlayerState != nullptr) ? true : false;
}

// 이미지 변경 함수
void UR1UserWidget::ChangeImage(UImage* ImageWidget, UTexture2D* NewTexture)
{
	if (ImageWidget && NewTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(NewTexture);
		ImageWidget->SetBrush(NewBrush);
	}
}

void UR1UserWidget::ChangeButtonImage(UButton* _buttonWidget, UTexture2D* NewTexture)
{
	if (_buttonWidget && NewTexture)
	{
		FSlateBrush NormalBrush;
		NormalBrush.SetResourceObject(NewTexture);

		FButtonStyle ButtonStyle = _buttonWidget->WidgetStyle;

		// 브러시 설정
		ButtonStyle.SetNormal(NormalBrush);         
		ButtonStyle.SetHovered(NormalBrush);
		ButtonStyle.SetPressed(NormalBrush);

		_buttonWidget->SetStyle(ButtonStyle);
	}
}

void UR1UserWidget::Set_ShowWidget(bool _show)
{
	if (!Canvas)
		return;
	
	if (true == _show)
	{
		if (ESlateVisibility::Visible != Canvas->GetVisibility())
			Canvas->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (ESlateVisibility::Hidden != Canvas->GetVisibility())
			Canvas->SetVisibility(ESlateVisibility::Hidden);
	}
}

ESlateVisibility UR1UserWidget::Get_ShowWidget()
{
	return Canvas->GetVisibility();
}

FVector2D UR1UserWidget::VInterpTo2D(FVector2D CurrentPos, FVector2D TargetPos, float DeltaTime, float InterpSpeed)
{
	CurrentPos.X = FMath::FInterpTo(CurrentPos.X, TargetPos.X, DeltaTime, InterpSpeed);
	CurrentPos.Y = FMath::FInterpTo(CurrentPos.Y, TargetPos.Y, DeltaTime, InterpSpeed);

	return CurrentPos; 
}

FVector2D UR1UserWidget::GetImagePosition(UImage* _image)
{
	if (_image)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_image->Slot);
		if (CanvasSlot)
			return CanvasSlot->GetPosition();
	}
	return FVector2D::ZeroVector;
}

void UR1UserWidget::MoveImage(UImage* _image, FVector2D NewPosition)
{
	if (_image)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_image->Slot);
		if (CanvasSlot)
			CanvasSlot->SetPosition(NewPosition);
	}
}

FVector2D UR1UserWidget::GetWindowSize()
{
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D WindowSize;
		GEngine->GameViewport->GetViewportSize(WindowSize);
		return WindowSize;
	}
	return FVector2D::ZeroVector;
}

FVector2D UR1UserWidget::GetImageSize(UImage* _Image)
{
	if (_Image)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_Image->Slot);
		if (CanvasSlot)
			return CanvasSlot->GetSize();
	}
	return FVector2D();
}


void UR1UserWidget::SetImageSize(UImage* _Image, FVector2D _size)
{
	if (_Image)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_Image->Slot);
		if (CanvasSlot)
			return CanvasSlot->SetSize(_size);
	}
}

void UR1UserWidget::SetImageSize_Outward(UImage* _Image, FVector2D _size)
{
	if (_Image)
	{
		FSlateBrush Brush = _Image->GetBrush();
		Brush.ImageSize = _size; // 브러시의 이미지 크기 설정
		_Image->SetBrush(Brush); // 변경된 브러시 적용
	}
}

FVector2D UR1UserWidget::GetTextPosition(UTextBlock* _text)
{
	if (_text)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_text->Slot);
		if (CanvasSlot)
			return CanvasSlot->GetPosition();
	}
	return FVector2D::ZeroVector;
}


FVector2D UR1UserWidget::RandomShake(FVector2D CurrentPosition, float ShakeAmount)
{
	float RandomX = FMath::FRandRange(-ShakeAmount, ShakeAmount);
	float RandomY = FMath::FRandRange(-ShakeAmount, ShakeAmount);

	FVector2D NewPosition = CurrentPosition + FVector2D(RandomX, RandomY);

	return NewPosition;
}

FVector2D UR1UserWidget::GetButtonSize(UButton* _button)
{
	if (_button)
	{
		UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(_button->Slot);
		if (ButtonSlot)
		{
			return ButtonSlot->GetSize();
		}
	}
	return FVector2D{};
}

void UR1UserWidget::SetButtonSize(UButton* _button, FVector2D _size)
{
	if (_button)
	{
		UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(_button->Slot);
		if (ButtonSlot)
		{
			ButtonSlot->SetSize(_size);
			//  ButtonSlot->SetPosition(FVector2D(X, Y));
		}
	}
}

void UR1UserWidget::MoveText(UTextBlock* _text, FVector2D NewPosition)
{
	if (_text)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_text->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(NewPosition);
		}
	}
}
