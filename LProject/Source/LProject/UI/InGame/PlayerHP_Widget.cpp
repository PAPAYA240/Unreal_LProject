

#include "UI/InGame/PlayerHP_Widget.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

UPlayerHP_Widget::UPlayerHP_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPlayerHP_Widget::NativeConstruct()
{
	bParentTick = false;
	Super::NativeConstruct();

	if (nullptr == CurrentPlayerState)
		return;

	Setting_NickName();

	if (Cross_Canvas)
	{
		for (UWidget* Child : Cross_Canvas->GetAllChildren())
		{
			UImage* Image = Cast<UImage>(Child);
			if (Image)
				EnergyArray.Emplace(Image);
		}
	}

	if (EnergyArray.Num() > 0)
	{
		int iEnergy = 0;
		for (auto* iter : EnergyArray)
		{
			if (IsValid(iter) && iter->GetVisibility() == ESlateVisibility::Visible)
			{

				if (CurrentPlayerState->Energy_Cnt - iEnergy >= CurrentPlayerState->Energy_Cnt)
					break;

				iEnergy++;
				iter->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UPlayerHP_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		// 1. 플레이어가 게임을 시작할 때 위치 세팅
		PlayerStart_Setting();

		// 2. 스케일 변환
		TranslateScaled();
	}
}

void UPlayerHP_Widget::Set_CanvasPos(int32 _cnt)
{
	// HPBar 위치 세팅
	if (nullptr == R1Player->Get_CurrentPlayerState())
		return;

	CurrentPlayerState = R1Player->Get_CurrentPlayerState();
	if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		for (UWidget* Child : Canvas->GetAllChildren())
		{
			if (Child)
			{
				// 현재 변환 가져오기
				FWidgetTransform WidgetTransform = Child->GetRenderTransform();
				FVector2D CurrentPosition = WidgetTransform.Translation;
				Canvas_Scaled = WidgetTransform.Scale;

				if (0 == _cnt)
				{
					Canvas_InitPos = CurrentPosition;
				}
				else if (1 == _cnt)
				{
					Canvas_InitPos = CurrentPosition;
					Canvas_InitPos.X += 600.f;
				}
				else if (2 == _cnt)
				{
					Canvas_InitPos = CurrentPosition;
					Canvas_InitPos.X += 1200.f;
				}
				else
					MSG_BOX(TEXT("UPlayerHP_Widget::Set_CanvasPos() : 존재하지 않는 플레이어 숫자입니다."));

				// 새 위치 계산
				FVector2D NewPosition = Canvas_InitPos + FVector2D(0.f, +700.f);

				// 새 위치로 설정
				WidgetTransform.Translation = NewPosition; // 새 위치로 변경
				Child->SetRenderTransform(WidgetTransform); // 변환 적용

				// 플레이어 비활성화시 Widget 위치 
				NonActive_Pos = Canvas_InitPos;
				NonActive_Pos.Y += 50.f;
			}
		}
	}
}

void UPlayerHP_Widget::Set_HpText(float _maxHp, float _hp)
{
	if (Hp_Text)
	{
		FString HpString = FString::Printf(TEXT("%.0f/%.0f"), _maxHp, _hp);
		Hp_Text->SetText(FText::FromString(HpString));
	}
}

void UPlayerHP_Widget::Set_AttackText(float _attack)
{
	if (Attack_Text)
	{
		FString AttackString = FString::Printf(TEXT("%.0f"), _attack);
		Attack_Text->SetText(FText::FromString(AttackString));
	}
}

void UPlayerHP_Widget::Set_HillText(float _hill)
{
	if (Hill_Text)
	{
		FString HillString = FString::Printf(TEXT("%.0f"), _hill);
		Hill_Text->SetText(FText::FromString(HillString));
	}
}

void UPlayerHP_Widget::Setting_NickName()
{
	if (EJOB_TYPE::HERBALIST == CurrentPlayerState->Player_Job)
		PlayerName_Text->SetText(FText::FromString(TEXT("약초상")));

	else if (EJOB_TYPE::WIZARD == CurrentPlayerState->Player_Job)
		PlayerName_Text->SetText(FText::FromString(TEXT("사냥꾼")));

	else if (EJOB_TYPE::LUMBERJACK == CurrentPlayerState->Player_Job)
		PlayerName_Text->SetText(FText::FromString(TEXT("나무꾼")));

	else
		PlayerName_Text->SetText(FText::FromString(TEXT("알 수 없음")));
}

void UPlayerHP_Widget::PlayerStart_Setting()
{
	if (R1Player->Get_GamePlayStart())
	{
		// 본인의 차례가 아니라면
		AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
		if (R1Player != GameMode->CurrentPlayer)
		{
			for (UWidget* Child : Canvas->GetAllChildren())
			{
				if (Child)
				{
					// 현재 변환 가져오기
					FWidgetTransform WidgetTransform = Child->GetRenderTransform();
					FVector2D CurrentPosition = WidgetTransform.Translation;
					FVector2D NewPos = VInterpTo2D(CurrentPosition, NonActive_Pos, GetWorld()->GetDeltaSeconds(), 2.f);

					// 새 위치로 설정
					WidgetTransform.Translation = NewPos; 
					Child->SetRenderTransform(WidgetTransform); 

					Adjustment_Opacity(true);
				}
			}
		}

		else if (R1Player == GameMode->CurrentPlayer)
		{
			if (R1Player->Get_GamePlayStart())
			{
				for (UWidget* Child : Canvas->GetAllChildren())
				{
					if (Child)
					{
						// 현재 변환 가져오기
						FWidgetTransform WidgetTransform = Child->GetRenderTransform();
						FVector2D CurrentPosition = WidgetTransform.Translation;
						FVector2D NewPos = VInterpTo2D(CurrentPosition, Canvas_InitPos, GetWorld()->GetDeltaSeconds(), 2.f);

						// 새 위치로 설정
						WidgetTransform.Translation = NewPos; // 새 위치로 변경
						Child->SetRenderTransform(WidgetTransform); // 변환 적용

						Adjustment_Opacity(false);
					}
				}
			}
		}
	}
}

void UPlayerHP_Widget::Adjustment_Opacity(bool _opacity)
{
	if (true == _opacity)
	{
		for (auto iter : Canvas->GetAllChildren())
		{
			if (UProgressBar* _progressBar = Cast<UProgressBar>(iter))
			{
				FLinearColor CurrentColor = _progressBar->GetFillColorAndOpacity();
				FLinearColor DarkenColor = CurrentColor * -GetWorld()->GetDeltaSeconds();
				if (DarkenColor.R < 0.3f)
					DarkenColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.3f);
				DarkenColor.A = 1.f;
				_progressBar->SetFillColorAndOpacity(DarkenColor);

				FLinearColor CurrentBackgroundColor = _progressBar->WidgetStyle.BackgroundImage.TintColor.GetSpecifiedColor();
				FLinearColor BrightenBackgroundColor = CurrentBackgroundColor * -GetWorld()->GetDeltaSeconds();
				if (BrightenBackgroundColor.R < 0.3f)
					BrightenBackgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.3f);
				BrightenBackgroundColor.A = 1.f;
				_progressBar->WidgetStyle.BackgroundImage.TintColor = FLinearColor(BrightenBackgroundColor.R, BrightenBackgroundColor.G, BrightenBackgroundColor.B, 1.f);
			}
			else if (UImage* Image = Cast<UImage>(iter))
			{
				FLinearColor CurrentColor = Image->GetColorAndOpacity();
				FLinearColor DarkenColor = CurrentColor * -GetWorld()->GetDeltaSeconds();
				if (DarkenColor.R < 0.3f)
					DarkenColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.3f);
				DarkenColor.A = 1.f;
				Image->SetColorAndOpacity(DarkenColor);
			}
			else if (UTextBlock* TextBlock = Cast<UTextBlock>(iter))
			{
				FLinearColor CurrentColor = TextBlock->ColorAndOpacity.GetSpecifiedColor();
				FLinearColor DarkenColor = CurrentColor * -GetWorld()->GetDeltaSeconds();
				if (DarkenColor.R < 0.3f)
					DarkenColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.3f);
				DarkenColor.A = 1.f;
				TextBlock->SetColorAndOpacity(DarkenColor);
			}
		}
	}
	
	else if (false == _opacity)
	{
		for (auto iter : Canvas->GetAllChildren())
		{
			if (UProgressBar* _progressBar = Cast<UProgressBar>(iter))
			{
				FLinearColor CurrentColor = _progressBar->GetFillColorAndOpacity(); 
				FLinearColor BrightenColor = CurrentColor * (1 + GetWorld()->GetDeltaSeconds()); 
				BrightenColor.A = 1.f;
				_progressBar->SetFillColorAndOpacity(BrightenColor);

				FLinearColor CurrentBackgroundColor = _progressBar->WidgetStyle.BackgroundImage.TintColor.GetSpecifiedColor(); 
				FLinearColor BrightenBackgroundColor = CurrentBackgroundColor * (1 + GetWorld()->GetDeltaSeconds()); 
				BrightenBackgroundColor.A = 1.f; 
				_progressBar->WidgetStyle.BackgroundImage.TintColor = FLinearColor(BrightenBackgroundColor.R, BrightenBackgroundColor.G, BrightenBackgroundColor.B, 1.f);
			}
			else if (UImage* Image = Cast<UImage>(iter))
			{
				FLinearColor CurrentColor = Image->GetColorAndOpacity();
				FLinearColor BrightenColor = CurrentColor * (1 + GetWorld()->GetDeltaSeconds());
				if (BrightenColor.R <= 0.f)
					BrightenColor = FLinearColor(0.f, 0.f, 0.f, 1.f);
				BrightenColor.A = 1.f;
				Image->SetColorAndOpacity(BrightenColor);
			}
			else if (UTextBlock* TextBlock = Cast<UTextBlock>(iter))
			{
				FLinearColor CurrentColor = TextBlock->ColorAndOpacity.GetSpecifiedColor();
				FLinearColor BrightenColor = CurrentColor * (1 + GetWorld()->GetDeltaSeconds());
				if (BrightenColor.R <= 0.f)
					BrightenColor = FLinearColor(0.f, 0.f, 0.f, 1.f);
				BrightenColor.A = 1.f;
				TextBlock->SetColorAndOpacity(BrightenColor);
			}
		}
	}
}

void UPlayerHP_Widget::TranslateScaled()
{
	// 1. 너무 빠른 스케일 변환을 막기 위해
	if (true == bScaleTranslate)
		ScaledTimer += GetWorld()->GetDeltaSeconds();

	// 2. hp가 변경될 때 크기를 좀더 키운다.
	if ((R1Player->Hp != R1Player->CurrentHp) ||
		(R1Player->MaxHp != R1Player->TargetMaxHp) ||
		(R1Player->CurrentAttack != R1Player->CurrentAttack))
	{
		ChangeScaled();
	}
	else
	{
		// 3. 스케일 원상복구
		if (ScaledTimer >= 2.f)
			ReturnScaled();
	}
}

void UPlayerHP_Widget::ChangeScaled()
{
	bScaleTranslate = true;

	for (UWidget* Child : Canvas->GetAllChildren())
	{
		if (Child)
		{
			FWidgetTransform WidgetTransform = Child->GetRenderTransform();

			float ScaleAmount = 0.5f * GetWorld()->GetDeltaSeconds();
			FVector2D ScaleIncrement(ScaleAmount, ScaleAmount);

			WidgetTransform.Scale += ScaleIncrement;

			FVector2D MaxScale(1.1f, 1.1f); 
			FVector2D MinScale(Canvas_Scaled.X, Canvas_Scaled.Y); 

			WidgetTransform.Scale.X = FMath::Clamp(WidgetTransform.Scale.X, MinScale.X, MaxScale.X);
			WidgetTransform.Scale.Y = FMath::Clamp(WidgetTransform.Scale.Y, MinScale.Y, MaxScale.Y);

			Child->SetRenderTransform(WidgetTransform);
		}
	}
}

void UPlayerHP_Widget::ReturnScaled()
{
	if (true == bScaleTranslate)
	{
		for (UWidget* Child : Canvas->GetAllChildren())
		{
			if (Child)
			{
				FWidgetTransform WidgetTransform = Child->GetRenderTransform();

				float ScaleAmount = 0.5f * GetWorld()->GetDeltaSeconds();
				FVector2D ScaleIncrement(ScaleAmount, ScaleAmount);
				WidgetTransform.Scale -= ScaleIncrement;

				FVector2D MaxScale(1.1f, 1.1f); // 최대 스케일
				FVector2D MinScale(Canvas_Scaled.X, Canvas_Scaled.Y); // 최소 스케일

				WidgetTransform.Scale.X = FMath::Clamp(WidgetTransform.Scale.X, MinScale.X, MaxScale.X);
				WidgetTransform.Scale.Y = FMath::Clamp(WidgetTransform.Scale.Y, MinScale.Y, MaxScale.Y);

				Child->SetRenderTransform(WidgetTransform);

				// 원래 상태로 돌아왔다면 false
				if (FVector2D::Distance(MinScale, WidgetTransform.Scale) <= KINDA_SMALL_NUMBER)
				{
					bScaleTranslate = false;
					ScaledTimer = 0.f;
				}
			}
		}
	}
}

void UPlayerHP_Widget::Set_HpProgressBar(float _percent)
{
	if (RedHPBar)
		RedHPBar->SetPercent(_percent);
}

bool UPlayerHP_Widget::Use_Energy()
{
	if (true == bFull_Energy) // 에너지가 가득 찼다면 return
		return false;

	if (EnergyArray.Num() > 0)
	{
		for (auto* iter : EnergyArray)
		{
			if (IsValid(iter) && iter->GetVisibility() == ESlateVisibility::Visible)
			{
				iter->SetVisibility(ESlateVisibility::Hidden);
				--CurrentPlayerState->Energy_Cnt;
				return true;
			}
		}
	}
	return false;
}

bool UPlayerHP_Widget::Add_Energy(int32 _addCnt)
{
	if (Cross_Canvas)
	{
		int32 iAddCnt = { 0 };
		for (auto iter : EnergyArray)
		{
			if (IsValid(iter) && iter->GetVisibility() == ESlateVisibility::Hidden)
			{
				if (iAddCnt >= _addCnt)
					return true;

				iter->SetVisibility(ESlateVisibility::Visible);
				++CurrentPlayerState->Energy_Cnt;
				++iAddCnt;
			}
		}
		return true;
	}
	return false;
}
