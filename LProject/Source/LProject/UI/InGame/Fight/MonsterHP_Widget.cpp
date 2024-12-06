

#include "UI/InGame/Fight/MonsterHP_Widget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

UMonsterHP_Widget::UMonsterHP_Widget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UMonsterHP_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_WidgetOption())
		MSG_BOX(TEXT("UMonsterHP_Widget() : Widget Option이 제대로 세팅되지 않았습니다."));
	
}

void UMonsterHP_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
		Operate_FightScene();

	else if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
		Operate_BossMap();
}

void UMonsterHP_Widget::Set_TargetMonster(AR1Monster* _monster)
{
	R1Monster = _monster;
	if(R1Monster  && MonsterName_Text)
		MonsterName_Text->SetText(R1Monster->Get_MonsterName());
}

void UMonsterHP_Widget::Set_ProgressBar(float _percent)
{
	if (_percent <= 0.f)
		return;

	if(ProgressBar)
		ProgressBar->SetPercent(_percent);
}

float UMonsterHP_Widget::Get_ProgressBar()
{
	if (ProgressBar)
		return ProgressBar->GetPercent();
	return 999.f;
}

void UMonsterHP_Widget::Set_HpText(float _maxHp, float _hp)
{
	if (Hp_Text)
	{
		FString HpString = FString::Printf(TEXT("%.0f/%.0f"), _maxHp, _hp);
		Hp_Text->SetText(FText::FromString(HpString));
	}
}

void UMonsterHP_Widget::Set_AttackText(float _attack)
{
	if (Attack_Text)
	{
		FString AttackString = FString::Printf(TEXT("%.0f"), _attack);
		Attack_Text->SetText(FText::FromString(AttackString));
	}
}

void UMonsterHP_Widget::Set_LevelText(float _level)
{
	if (Level_Text)
	{
		FString LevelString = FString::Printf(TEXT("%.0f"), _level);
		Level_Text->SetText(FText::FromString(LevelString));
	}
}

bool UMonsterHP_Widget::Setting_WidgetOption()
{
	if (UCanvasPanel* MonsterHpCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("MonsterHp_Canvas"))))
		MonsterHpCanvas->SetVisibility(ESlateVisibility::Hidden);
	else
		return false;
	if (VS_Image)
	{
		VS_OriginScaled = GetImageSize(VS_Image);
		VS_OriginPos = GetImagePosition(VS_Image);
		SetImageSize(VS_Image, FVector2D(0.f, 0.f));
		MoveImage(VS_Image, FVector2D(0.f, 0.f));
	}
	else
		return false;

	return true;
}

void UMonsterHP_Widget::Operate_BossMap()
{
	if (R1Monster == CurrentPlayerState->TargetMonster)
		Set_ShowWidget(true);
	else
		Set_ShowWidget(false);
}

void UMonsterHP_Widget::Operate_FightScene()
{
	// 1. Operate Vs Image 
	if (false == bFightStart)
	{
		if (true == Opreate_VSImage())
		{
			if (UCanvasPanel* MonsterHpCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("MonsterHp_Canvas"))))
				MonsterHpCanvas->SetVisibility(ESlateVisibility::Visible);

			bFightStart = true;
		}
	}

	if (AR1Monster* FightMonster = R1Player->Get_CurrentPlayerState()->TargetMonster)
	{
		if(R1Monster == FightMonster)
		{
			if (ESlateVisibility::Visible != Get_ShowWidget())
				Set_ShowWidget(true);
		}
		else
		{
			if (ESlateVisibility::Hidden != Get_ShowWidget())
				Set_ShowWidget(false);
		}
	}
}

bool UMonsterHP_Widget::Opreate_VSImage()
{
	if (ESlateVisibility::Visible == VS_Image->GetVisibility())
	{
		VS_Timer += GetWorld()->GetDeltaSeconds();
		if (4.f <= VS_Timer)
		{
			FVector2D TargetScaled = { VS_OriginScaled.X, 0.f };
			FVector2D NewScaled = VInterpTo2D(GetImageSize(VS_Image), TargetScaled, GetWorld()->GetDeltaSeconds(), 20.f);
			SetImageSize(VS_Image, NewScaled);

			if (NewScaled.Y <= 50.f)
				bVSEnd = true;

			if (NewScaled.Y <= 0.f)
			{
				VS_Image->SetVisibility(ESlateVisibility::Hidden);
				return true;
			}
		}
		else if (3.f <= VS_Timer)
		{
			if (false == bScaledChange)
			{
				// 이미지 키우기
				FVector2D BigScaled = VS_OriginScaled;
				BigScaled.X += 400.f;
				BigScaled.Y += 400.f;
				FVector2D NewScaled = VInterpTo2D(GetImageSize(VS_Image), BigScaled, GetWorld()->GetDeltaSeconds(), 20.f);
				SetImageSize(VS_Image, NewScaled);
				FVector2D NewPos = VInterpTo2D(GetImagePosition(VS_Image), FVector2D(-600.f, -600.f), GetWorld()->GetDeltaSeconds(), 20.f);
				MoveImage(VS_Image, NewPos);

				if (20.f >= FVector2D::Distance(NewScaled, BigScaled))
					bScaledChange = true;
			}
			else if (true == bScaledChange)
			{
				// 이미지 키우기
				FVector2D NewScaled = VInterpTo2D(GetImageSize(VS_Image), VS_OriginScaled, GetWorld()->GetDeltaSeconds(), 15.f);
				SetImageSize(VS_Image, NewScaled);
				FVector2D NewPos = VInterpTo2D(GetImagePosition(VS_Image), VS_OriginPos, GetWorld()->GetDeltaSeconds(), 15.f);
				MoveImage(VS_Image, NewPos);

				if (1.f >= FVector2D::Distance(NewPos, VS_OriginScaled))
					bScaledChange = true;
			}

			// 랜덤 흔들기
			MoveImage(VS_Image, RandomShake(GetImagePosition(VS_Image), 4.f));
		}
	}
	return false;
}
