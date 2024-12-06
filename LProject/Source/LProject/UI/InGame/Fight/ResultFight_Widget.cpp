
#include "UI/InGame/Fight/ResultFight_Widget.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

UResultFight_Widget::UResultFight_Widget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UResultFight_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Left_Image)
		Left_InitPos = GetImagePosition(Left_Image);

	if (Right_Image)
		Right_InitPos = GetImagePosition(Right_Image);

	FVector2D WindowSize = GetWindowSize();

	Left_TargetPos = FVector2D(WindowSize.X / 2.f - GetImageSize(Left_Image).X / 2.f, WindowSize.Y / 2.f);
	Left_TargetPos.Y = Left_InitPos.Y;

	Right_TargetPos = FVector2D(WindowSize.X / 2.f - GetImageSize(Right_Image).X / 2.f, WindowSize.Y / 2.f);
	Right_TargetPos.Y = Right_InitPos.Y;
}

void UResultFight_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!R1Player || !CurrentPlayerState)
		return;

	if(ESCENE::FIGHT_SCENE == R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
	{
		Fight_SceneWidget();
	}
	else if (ESCENE::INGAME == R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
	{
		InGame_SceneWidget();
	}
}

void UResultFight_Widget::Image_Moving(float DeltaTime)
{
	if(false == bBetweenTrum)
	{
		if (Left_Image)
		{
			FVector2D CurrentPos = GetImagePosition(Left_Image);
			FVector2D NewPos = VInterpTo2D(CurrentPos, Left_TargetPos, DeltaTime, 2.f);
			MoveImage(Left_Image, NewPos);

			if (50.f >= FVector2D::Distance(NewPos, Left_TargetPos))
				bBetweenTrum = true;
		}

		if (Right_Image)
		{
			FVector2D CurrentPos = GetImagePosition(Right_Image);
			FVector2D NewPos = VInterpTo2D(CurrentPos, Right_TargetPos, DeltaTime, 2.f);
			MoveImage(Right_Image, NewPos);

			if (50.f >= FVector2D::Distance(NewPos, Right_TargetPos))
				bBetweenTrum = true;
		}
	}
	else if (true == bBetweenTrum)
	{
		R1Player->Set_FightAuthority(EFIGHT_AUTHORITY::END);

		if (Left_Image)
		{
			FVector2D CurrentPos = GetImagePosition(Left_Image);
			FVector2D LTargetPos = CurrentPos;
			LTargetPos.X = GetWindowSize().X + GetImageSize(Left_Image).X;
			
			FVector2D NewPos = VInterpTo2D(CurrentPos, LTargetPos, DeltaTime, 3.f);
			MoveImage(Left_Image, NewPos);

			if (50.f >= FVector2D::Distance(NewPos, LTargetPos))
				Set_ShowWidget(false);
		}

		if (Right_Image)
		{
			FVector2D CurrentPos = GetImagePosition(Right_Image);
			FVector2D RTargetPos = CurrentPos;
			RTargetPos.X = -GetImageSize(Left_Image).X - 1000.f;

			FVector2D NewPos = VInterpTo2D(CurrentPos, RTargetPos, DeltaTime, 3.f);
			MoveImage(Right_Image, NewPos);

			if (50.f >= FVector2D::Distance(NewPos, RTargetPos))
			{
				Set_ShowWidget(false);
				bOperateWidget = false;
				bBetweenTrum = false;
				MoveImage(Left_Image, Left_InitPos);
				MoveImage(Right_Image, Right_InitPos);
			}
		}
	}
}

void UResultFight_Widget::Fight_SceneWidget()
{
	if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
		if (true == R1Player->Get_EndFight())
			bOperateWidget = true;

		if (true == bOperateWidget)
		{
			if (ESlateVisibility::Visible != Get_ShowWidget())
			{
				Set_ShowWidget(true);
				/* Player의 승리라면*/
				if (false == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)])
				{
					if (WinSound)
						UGameplayStatics::PlaySound2D(GetWorld(), WinSound);
					Result_Text->SetText(FText::FromString(TEXT("대차게 승리하였다!")));
				}
				else
					Result_Text->SetText(FText::FromString(TEXT("죽어버렸다..")));
			}

			Image_Moving(GetWorld()->GetDeltaSeconds());
		}
	}
}

void UResultFight_Widget::InGame_SceneWidget()
{
	if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		if (true == bOperateWidget)
		{
			if (ESlateVisibility::Visible != Get_ShowWidget())
			{
				Set_ShowWidget(true);
				//if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)])
				{
					Result_Text->SetText(FText::FromString(TEXT("풍덩 빠져버렸다...람쥐.")));
				}
			}
			Image_Moving(GetWorld()->GetDeltaSeconds());
		}
	}
}
