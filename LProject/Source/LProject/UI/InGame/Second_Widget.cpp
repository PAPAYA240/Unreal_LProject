

#include "UI/InGame/Second_Widget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Camera/CameraManager.h"

#include "Player/Player_Herblist/R1Player_Herbalist.h"

USecond_Widget::USecond_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USecond_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_Widget())
		MSG_BOX(TEXT("USecond_Widget() : Widget 세팅에 실패했습니다."));

	if(!Setting_Position())
		MSG_BOX(TEXT("USecond_Widget() : Position 세팅에 실패했습니다."))
}

void USecond_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (true == bFirstSceneEnd)
		ChangeChildWidgetsColorToBlack();
	
	if (true == bGameStart)
	{
		if(false == bInitCamer_Setting)
		{
			ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
			bInitCamer_Setting = MyCameraManager->Initialize_CameraPos();
		}
		else
		{
			Set_ShowWidget(true);
			if (Random_Second(InDeltaTime))
			{
				if (!bMovingStart)
				{
					if (Random_Move(InDeltaTime))
						Ready_Moving(InDeltaTime);
				}
				else
				{
					Start_Moving(InDeltaTime);
				}
			}
		}
	}

	Target_BackGround->SetVisibility(ESlateVisibility::Hidden);
	Target_SecondText->SetVisibility(ESlateVisibility::Hidden);
}


// 랜덤으로 초 돌리기 : 나중에 스피드에 따라서 초 범위 넓히기
bool USecond_Widget::Random_Second(float DeltaTime)
{
	if (true == bGameReady)
		return true;

	Random_Timer += DeltaTime;

	if (Random_Timer >= 1.5f)
	{
		SinceLastUpdateTimer += DeltaTime;

		if (UpdateInterval <= 0.f)
			UpdateInterval = 0.1f;

		if (SinceLastUpdateTimer >= UpdateInterval)
		{
			RandomTime = FMath::FRandRange(3.5f, 10.5f);
			FString RandomTimeString = FString::Printf(TEXT("%.2f"), RandomTime);
			FText RandomTimeText = FText::FromString(RandomTimeString);

			if (Second_Text)
				Second_Text->SetText(RandomTimeText);

			SinceLastUpdateTimer = 0.f;
			UpdateInterval += DeltaTime * 2.f; 
		}

		if (Random_Timer >= 3.f)
		{
			bGameReady = true;
			return true;
		}
	}
	else
	{
		Random_StartSetting();

		RandomTime = FMath::FRandRange(3.5f, 10.f);
		FString RandomTimeString = FString::Printf(TEXT("%.2f"), RandomTime);
		//RandomTime = FMath::RandRange(2, 6);
		//FString RandomTimeString = FString::Printf(TEXT("%d"), RandomTime);
		FText RandomTimeText = FText::FromString(RandomTimeString);
		Second_Text->SetText(RandomTimeText);
	}
	return false;
}

bool USecond_Widget::Random_Move(float DeltaTime)
{
	Guidance_Text->SetVisibility(ESlateVisibility::Hidden);

	FVector2D CurrentPos = GetImagePosition(Second_BackGround);
	FVector2D CurrentTextPos = GetTextPosition(Second_Text);

	FVector2D NewPos = VInterpTo2D(CurrentPos, Target_ImagePos, DeltaTime, 3.f);
	FVector2D NewTextPos = VInterpTo2D(CurrentTextPos, Target_TextPos, DeltaTime, 3.f);

	MoveImage(Second_BackGround, NewPos);
	MoveText(Second_Text, NewTextPos);

	if (FVector2D::Distance(NewPos, Target_ImagePos))
		return true;
	return false;
}

void USecond_Widget::Random_StartSetting()
{
	if (ESlateVisibility::Visible != Second_Text->GetVisibility())
	{
		Second_Text->SetVisibility(ESlateVisibility::Visible);
		Guidance_Text->SetVisibility(ESlateVisibility::Visible);
		Second_BackGround->SetVisibility(ESlateVisibility::Visible);
	}
}

bool USecond_Widget::Second_Configure()
{
	if(Moving_Ready_Text)
		Moving_Ready_Text->SetVisibility(ESlateVisibility::Hidden);

	if(Second_BackGround)
		Second_BackGround->SetVisibility(ESlateVisibility::Hidden);

	if (Second_Text)
		Second_Text->SetVisibility(ESlateVisibility::Hidden);

	if(Second_BackGround)
		MoveImage(Second_BackGround, Init_ImagePos);

	if (Second_Text)
		MoveText(Second_Text, Init_TextPos);

	Random_Timer = { 0.f };
	UpdateInterval = 0.5f;
	SinceLastUpdateTimer = 0.f;
	MovingStart_Timer = 0.f;
	MoveingReady_Timer = 0.f;

	bGameStart = { false };
	bGameReady = { false };
	bMovingStart = { false };
	bInitCamer_Setting = false;
	bZoomIn = false;

	CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = false;
	return true;
}

bool USecond_Widget::Ready_Moving(float DeltaTime)
{
	if (bMovingStart)
		return true;

	if (MoveingReady_Timer >= 1.f)
	{
		Moving_Ready_Text->SetText(FText::FromString(TEXT("시작!")));
		MovingStart_Timer += DeltaTime;
		if(MovingStart_Timer >= 0.5f)
		{	
			// Moveing 시작
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = true;
			MovingStart_Timer = 0.f;
			bMovingStart = true;

			if (Moving_Ready_Text)
				Moving_Ready_Text->SetVisibility(ESlateVisibility::Hidden);

			return true;
		}
	}
	else
	{
		Moving_Ready_Text->SetVisibility(ESlateVisibility::Visible);

		MoveingReady_Timer += DeltaTime;
		Moving_Ready_Text->SetText(FText::FromString(TEXT("준비")));
	}
	return false; 
}

void USecond_Widget::Start_Moving(float DeltaTime)
{
	if (RandomTime <= 0)
	{
		Moving_Ready_Text->SetVisibility(ESlateVisibility::Visible);
		Moving_Ready_Text->SetText(FText::FromString(TEXT("그만!")));
		CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = false;

		/* 턴이 끝났을 때 플레이어가 가진 운에 의해 플레이어의 기초 능력이 발동된다.
		1. 대장장이 :
		2. 연금술사 :
		3. 약초상 : 약초 발견
		*/
		// 1. 능력을 사용할 것인가?
		Distinction_Ability();
		
		// 2. 능력 사용 함
		if (true == bUseAbility)
		{
			if (Use_Ability())
			{
				bUseAbility = false;
				bPlayer_TurnAbility = false;
				bUseAbility_Distinction = false;
			}
		}
		// 3. 사용 안 함
		else if(false == bUseAbility)
		{
			if (TurnEndZoomTransition())
			{
				bUseAbility = false;
				bUseAbility_Distinction = false;
			}
		}
	}
	else
	{
		Moving_Ready_Text->SetVisibility(ESlateVisibility::Hidden);
		RandomTime -= DeltaTime;
	}

	if(RandomTime <= 0)
	{
		RandomTime = 0.f;
		Second_Text->SetText(FText::FromString(TEXT("0.00")));
	}
	else
	{	
		FString RandomTimeString = FString::Printf(TEXT("%.2f"), RandomTime);
		FText RandomTimeText = FText::FromString(RandomTimeString);
		Second_Text->SetText(RandomTimeText);
	}
}

void USecond_Widget::Distinction_Ability()
{
	if (true == bUseAbility_Distinction)
		return;

	bUseAbility_Distinction = true;
	if (AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (R1Player->CalculateAbility((ABILITY_TYPE::LUCKY)))
		{
			MSG_BOX(TEXT("약초 능력 발동"));
			bUseAbility = true;
		}
		else
		{
			bUseAbility = false;
			MSG_BOX(TEXT("약초 능력 미발동"));
		}
	}
}

bool USecond_Widget::Use_Ability()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	switch (GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_Job)
	{
	case EJOB_TYPE::HERBALIST:
	{
		AR1Player_Herbalist* HerbPlayer = Cast<AR1Player_Herbalist>(GameMode->CurrentPlayer);
		if (false == bPlayer_TurnAbility)
		{
			HerbPlayer->Set_FindHerb(true);
			bPlayer_TurnAbility = true;
		}

		if (false == HerbPlayer->Get_FindHerb())
		{
			if (TurnEndZoomTransition())
				return true;
		}
		break;
	}
	default:
		break;
	}
	return false;
}

// Scene First Story 에 관련된 함수
void USecond_Widget::OnContinueButtonClicked()
{
	bFirstSceneEnd = true;
}

// Scene First Story 가 Black -> Opertiy
void USecond_Widget::ChangeChildWidgetsColorToBlack()
{
	if (!FirstSceneStory_Instance) return; // FirstSceneStory_Instance가 nullptr인지 확인

	UCanvasPanel* SceneCanvas = Cast<UCanvasPanel>(FirstSceneStory_Instance->GetWidgetFromName(TEXT("Canvas")));
	if (!SceneCanvas) return; // SceneCanvas가 nullptr인지 확인

	if (!bBlackColor && ColorChange_Timer <= 0.f)
	{
		bBlackColor = true;
		ColorChange_Timer = 1.f;
	}

	ColorChange_Timer -= GetWorld()->GetDeltaSeconds();

	for (UWidget* Child : SceneCanvas->GetAllChildren())
	{
		if (Child)
		{
			if (!bBlackColor)
			{
				// 이미지 
				UImage* ImageWidget = Cast<UImage>(Child);
				if (ImageWidget)
				{
					FLinearColor CurrentColor = ImageWidget->GetColorAndOpacity();
					FLinearColor NewColor(ColorChange_Timer, ColorChange_Timer, ColorChange_Timer, CurrentColor.A); // 알파는 기존 값 유지
					ImageWidget->SetColorAndOpacity(NewColor);
				}

				// 버튼
				UButton* ButtonWidget = Cast<UButton>(Child);
				if (ButtonWidget)
				{
					FLinearColor CurrentColor = ButtonWidget->WidgetStyle.Normal.TintColor.GetSpecifiedColor();
					FLinearColor NewColor(ColorChange_Timer, ColorChange_Timer, ColorChange_Timer, CurrentColor.A);

					FButtonStyle ButtonStyle = ButtonWidget->WidgetStyle;
					ButtonStyle.Normal.TintColor = FSlateColor(NewColor);
					ButtonStyle.Hovered.TintColor = FSlateColor(NewColor);
					ButtonStyle.Pressed.TintColor = FSlateColor(NewColor);

					ButtonWidget->SetStyle(ButtonStyle);

					for (auto iter : ButtonWidget->GetAllChildren())
					{
						UTextBlock* ButtonText = Cast<UTextBlock>(iter);
						if (ButtonText)
						{
							FLinearColor TextColor = ButtonText->ColorAndOpacity.GetSpecifiedColor();
							FLinearColor NewTextColor(ColorChange_Timer, ColorChange_Timer, ColorChange_Timer, TextColor.A);
							ButtonText->SetColorAndOpacity(NewTextColor);
						}
					}
				}

				// 텍스트
				UTextBlock* TextWidget = Cast<UTextBlock>(Child);
				if (TextWidget)
				{
					FLinearColor CurrentColor = TextWidget->ColorAndOpacity.GetSpecifiedColor();
					FLinearColor NewColor(ColorChange_Timer, ColorChange_Timer, ColorChange_Timer, CurrentColor.A);
					TextWidget->SetColorAndOpacity(NewColor);
				}
			}
			else if (bBlackColor)
			{
				// 이미지
				UImage* ImageWidget = Cast<UImage>(Child);
				if (ImageWidget)
				{
					FLinearColor NewColor = ImageWidget->GetColorAndOpacity();
					NewColor.A = ColorChange_Timer; // 투명도 조정
					ImageWidget->SetColorAndOpacity(NewColor);
				}

				// 버튼
				UButton* ButtonWidget = Cast<UButton>(Child);
				if (ButtonWidget)
				{
					FButtonStyle ButtonStyle = ButtonWidget->WidgetStyle;

					FLinearColor NormalColor = ButtonStyle.Normal.TintColor.GetSpecifiedColor();
					NormalColor.A = ColorChange_Timer;
					ButtonStyle.Normal.TintColor = FSlateColor(NormalColor);

					FLinearColor HoveredColor = ButtonStyle.Hovered.TintColor.GetSpecifiedColor();
					HoveredColor.A = ColorChange_Timer;
					ButtonStyle.Hovered.TintColor = FSlateColor(HoveredColor);

					FLinearColor PressedColor = ButtonStyle.Pressed.TintColor.GetSpecifiedColor();
					PressedColor.A = ColorChange_Timer;
					ButtonStyle.Pressed.TintColor = FSlateColor(PressedColor);

					ButtonWidget->SetStyle(ButtonStyle);

					for (auto iter : ButtonWidget->GetAllChildren())
					{
						UTextBlock* ButtonText = Cast<UTextBlock>(iter);
						if (ButtonText)
						{
							FLinearColor TextColor = ButtonText->ColorAndOpacity.GetSpecifiedColor();
							TextColor.A = ColorChange_Timer; 
							ButtonText->SetColorAndOpacity(TextColor);
						}
					}
				}

				// 텍스트
				UTextBlock* TextWidget = Cast<UTextBlock>(Child);
				if (TextWidget)
				{
					FLinearColor TextColor = TextWidget->ColorAndOpacity.GetSpecifiedColor();
					TextColor.A = ColorChange_Timer; // 투명도 조정
					TextWidget->SetColorAndOpacity(TextColor);
				}

				if(0.f >= ColorChange_Timer)
				{
					ColorChange_Timer = 0.f;
					bFirstSceneEnd = false;
				}
			}
		}
	}
}

bool USecond_Widget::Setting_Widget()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (ESCENE::BOSS_SCENE == GameMode->PlayerArray[0]->Get_CurrentPlayerState()->Player_PrevScene)
	{
		/* Player Setting Widget */
		FirstSceneStory_Instance = Cast<UUserWidget>(CreateWidget(GetWorld(), FirstSceneStory_Widget));
		if (FirstSceneStory_Instance)
			FirstSceneStory_Instance->AddToViewport();
		else
		{
			MSG_BOX(TEXT("Failed : FirstSceneStory_Instance"));
			return false;
		}
	}

	if (FirstSceneStory_Instance)
	{
		UButton* ContinueButton = Cast<UButton>(FirstSceneStory_Instance->GetWidgetFromName(TEXT("Button")));
		if (ContinueButton)
			ContinueButton->OnClicked.AddDynamic(this, &USecond_Widget::OnContinueButtonClicked);
		else
			return false;
	}
	return true;
}

bool USecond_Widget::Setting_Position()
{
	FVector2D  ViewportCenter = GetWindowSize();
	Target_ImagePos = GetImagePosition(Target_BackGround);
	Target_TextPos = GetTextPosition(Target_SecondText);

	Init_ImagePos = GetImagePosition(Second_BackGround);
	Init_TextPos = GetTextPosition(Second_Text);

	Moving_Ready_Text->SetVisibility(ESlateVisibility::Hidden);
	return true;
}

bool USecond_Widget::TurnEndZoomTransition()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ACameraManager* MyCameraManager={};
		if (PlayerController && PlayerController->PlayerCameraManager)
			MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
		if (!MyCameraManager)
			return false;
		// 1. 줌 인 후
		if (!bZoomIn)
		{
			bZoomIn = MyCameraManager->CameraZoomIn(1.f);
		}
		// 2. 줌 아웃해서 배경 보여주기
		else if (bZoomIn)
		{
			if (true == MyCameraManager->CameraZoomOut(2500.f))
			{
				// 맵 보여주기 or 재정비 시간
				bMouseMoving = true;
				Second_Configure();
				return true;
			}
		}
	}
	return false;
}

