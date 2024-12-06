#include "UI/Lobby/PlayerSetting_Widget.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/R1LoadPlayer.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"

#include "Kismet/GameplayStatics.h" 
#include "System/R1GameInstance.h"
#include "R1GameModeBase.h"

#include "ClassInfo_Widget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"


UPlayerSetting_Widget::UPlayerSetting_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPlayerSetting_Widget::Set_ShowWidget(ESlateVisibility _visible)
{
	Main_Canvas->SetVisibility(_visible);
}

void UPlayerSetting_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	Super::Set_ShowWidget(true);

	if (!Setting_Widget())
		MSG_BOX(TEXT("UPlayerSetting_Widget() : Widget Setting이 되지 않았습니다."));
}

void UPlayerSetting_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Show_ClassInfo();

	// 메인이 켜졌을 때
	if(ESlateVisibility::Visible == Main_Canvas->GetVisibility())
		SelectPlayer = PlayerClassInfo_Instance->InitPos_Setting();

	// 커스텀이 켜졌을 때
	if (ESlateVisibility::Visible == Custom_Canvas->GetVisibility())
	{
		Main_Canvas->SetVisibility(ESlateVisibility::Hidden);
		PlayerClassInfo_Instance->Camera_Setting(true);

		Change_Customize();
	}
}

void UPlayerSetting_Widget::Show_ClassInfo()
{
	// 안 보일 때는 Class 켜기
	if (ESlateVisibility::Hidden == Main_Canvas->GetVisibility() && ESlateVisibility::Hidden == Custom_Canvas->GetVisibility())
	{
		if (PlayerClassInfo_Instance)
			SelectJob_Text->SetText(PlayerClassInfo_Instance->Get_JobName());

		PlayerClassInfo_Instance->Set_ShowWidget(true);
		PlayerClassInfo_Instance->Camera_Setting(true);
	}
	// 보일 땐 Class 끄기
	else if (ESlateVisibility::Visible == Main_Canvas->GetVisibility())
	{
		Super::Set_ShowWidget(true);
		Main_Canvas->SetVisibility(ESlateVisibility::Visible);
		PlayerClassInfo_Instance->Set_ShowWidget(false);
		PlayerClassInfo_Instance->Camera_Setting(false);
	}
}

void UPlayerSetting_Widget::Change_Customize()
{
	switch (Custom_Cnt)
	{
	case static_cast<int>(CUSTOMIZE_PLAYER::HAIR_CUSTOM):
		Hair_Canvas->SetVisibility(ESlateVisibility::Visible);
		Top_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Skin_Canvas->SetVisibility(ESlateVisibility::Hidden);
		break;

	case static_cast<int>(CUSTOMIZE_PLAYER::FACE_CUSTOM):
		Hair_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Top_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Skin_Canvas->SetVisibility(ESlateVisibility::Visible);
		break;

	case static_cast<int>(CUSTOMIZE_PLAYER::TOB_CUSTOM):
		Hair_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Top_Canvas->SetVisibility(ESlateVisibility::Visible);
		Skin_Canvas->SetVisibility(ESlateVisibility::Hidden);
		break;

	default:
		Hair_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Top_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Skin_Canvas->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
}
	
void UPlayerSetting_Widget::OnSelectJobClicked()
{
	// 플레이어를 선택하지 않았다면 제외
	if(true == PlayerClassInfo_Instance->Get_IsSelectPlayer())
	{
		Main_Canvas->SetVisibility(ESlateVisibility::Hidden);
		Super::Set_ShowWidget(false);
	}
}

void UPlayerSetting_Widget::OnStartClicked()
{
	// InGame 들어갈 때 Player 순서를 정해줄 것이다 2 1 3
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	TArray<AR1Player*> PArray;

	for (int i = 0; i < LAST_PLAYER; i++)
		PArray.Emplace(GameMode->PlayerArray[i]);
	PArray.Sort([](const AR1Player& A, const AR1Player& B)
		{
			return A.GetActorLocation().Y < B.GetActorLocation().Y; 
		});

	TArray<AMyPlayerState*> PState;
	PState.Emplace(PArray[1]->Get_CurrentPlayerState());
	PState.Emplace(PArray[0]->Get_CurrentPlayerState());
	PState.Emplace(PArray[2]->Get_CurrentPlayerState());

	for (int i = 0; i < PState.Num(); i++)
		GameMode->PlayerStates[i] = PState[i];

	if (R1Player)
	{
		R1Player->Change_Level(ESCENE::INGAME);
		// R1Player->Change_Level(ESCENE::BOSS_SCENE);
	}
}

void UPlayerSetting_Widget::OnCustomizeClicked()
{
	Main_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Custom_Canvas->SetVisibility(ESlateVisibility::Visible);
	Hair_Canvas->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerSetting_Widget::OnCustomLeftClicked()
{
	--Custom_Cnt;

	if (Custom_Cnt < 0)
		Custom_Cnt = static_cast<int>(CUSTOMIZE_PLAYER::END_CUSTOM) - 1;
}

void UPlayerSetting_Widget::OnCustomRightClicked()
{
	++Custom_Cnt;

	if (Custom_Cnt > static_cast<int>(CUSTOMIZE_PLAYER::END_CUSTOM) - 1)
		Custom_Cnt = 0;
}

void UPlayerSetting_Widget::OnExitClicked()
{
	Custom_Canvas->SetVisibility(ESlateVisibility::Hidden);
	Main_Canvas->SetVisibility(ESlateVisibility::Visible);
	PlayerClassInfo_Instance->Set_SelectPlayer(false);
}


bool UPlayerSetting_Widget::Setting_Widget()
{
	/* Class Info */
	PlayerClassInfo_Instance = Cast<UClassInfo_Widget>(CreateWidget(GetWorld(), PlayerClassInfo_Widget));
	if (PlayerClassInfo_Instance)
	{
		PlayerClassInfo_Instance->AddToViewport();
		PlayerClassInfo_Instance->Get_PlayerSettingClass_Ptr(this);
	}
	else
	{
		MSG_BOX(TEXT("Failed : PlayerHP_Instance"));
		return false;
	}


	if (UButton* SelectJobButton = Cast<UButton>(GetWidgetFromName(TEXT("SelectJob_Button"))))
		SelectJobButton->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnSelectJobClicked);
	else
		return false;

	if (UButton* StartButton = Cast<UButton>(GetWidgetFromName(TEXT("Start_Button"))))
		StartButton->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnStartClicked);
	else
		return false;

	if (UButton* CustomizeButton = Cast<UButton>(GetWidgetFromName(TEXT("Customize_Button"))))
		CustomizeButton->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnCustomizeClicked);
	else
		return false;

	if (UButton* CustomRightButton = Cast<UButton>(GetWidgetFromName(TEXT("Custom_RightButton"))))
		CustomRightButton->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnCustomRightClicked);
	else
		return false;

	if (UButton* CustomLeftButton = Cast<UButton>(GetWidgetFromName(TEXT("Custom_LeftButton"))))
		CustomLeftButton->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnCustomLeftClicked);
	else
		return false;

	if (UButton* ButtonExit = Cast<UButton>(GetWidgetFromName(TEXT("Button_Exit"))))
		ButtonExit->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnExitClicked);
	else
		return false;

	// 1. Hair
	int32 ButtonIndex = 0;
	for (auto& iter : Hair_Canvas->GetAllChildren())
	{
		UButton* ButtonType = Cast<UButton>(iter);
		if (ButtonType)
		{
			HairButtonArray.Emplace(ButtonType);
			ButtonType->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnHairButtonClicked);
			ButtonIndex++;
		}
	}

	// 2. face
	ButtonIndex = 0;
	for (auto& iter : Skin_Canvas->GetAllChildren())
	{
		UButton* ButtonType = Cast<UButton>(iter);
		if (ButtonType)
		{
			FaceButtonArray.Emplace(ButtonType);
			ButtonType->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnFaceButtonClicked);
			ButtonIndex++;
		}
	}

	// 3. Top
	ButtonIndex = 0;
	for (auto& iter : Top_Canvas->GetAllChildren())
	{
		UButton* ButtonType = Cast<UButton>(iter);
		if (ButtonType)
		{
			TopButtonArray.Emplace(ButtonType);
			ButtonType->OnClicked.AddDynamic(this, &UPlayerSetting_Widget::OnTopButtonClicked);
			ButtonIndex++;
		}
	}

	Custom_Cnt = static_cast<int32>(CUSTOMIZE_PLAYER::FACE_CUSTOM);
	return true;
}


void UPlayerSetting_Widget::OnHairButtonClicked()
{
	int iCnt = { 0 };
	for (auto iter : HairButtonArray)
	{
		if (iter->IsHovered())
		{
			SelectPlayer->Player_MaterialCustomize(iCnt, Custom_Cnt, this);
			break;
		}
		iCnt++;
	}
}

void UPlayerSetting_Widget::OnFaceButtonClicked()
{
	int iCnt = { 0 };
	for (auto iter : FaceButtonArray)
	{
		if (iter->IsHovered())
		{
			SelectPlayer->Player_MaterialCustomize(iCnt, Custom_Cnt, this);
			break;
		}
		iCnt++;
	}
}

void UPlayerSetting_Widget::OnTopButtonClicked()
{
	int iCnt = { 0 };
	for (auto iter : TopButtonArray)
	{
		if (iter->IsHovered())
		{
			SelectPlayer->Player_MaterialCustomize(iCnt, Custom_Cnt, this);
			break;
		}
		iCnt++;
	}
}
