

#include "UI/InGame/Fight/ReadyFight_Widget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "System/R1GameInstance.h"
#include "Kismet/GameplayStatics.h" 
#include "Monster/R1Monster.h"

#include "R1GameModeBase.h"

#define READY_TIME				0.5f	// Energy가 업데이트 되는 간격
#define ENERGY_CNT				3

UReadyFight_Widget::UReadyFight_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UReadyFight_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_Widget())
		MSG_BOX(TEXT("URadyFight_Widget() : 위젯이 세팅되지 않았습니다."));
}

void UReadyFight_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (false == bOpenWidget)
	{
		bOpenWidget = Show_Widget();

		if (!Setting_Text())
			MSG_BOX(TEXT("UReadyFight_Widget() : Text를 찾지 못했습니다."));

	}
	Hovering_RandomAbilityWidget();

	if (true == bBehavior[static_cast<int32>(READY_TYPE::AMBUSH)])
		Ambush_Probability(InDeltaTime);

	else if (true == bBehavior[static_cast<int32>(READY_TYPE::RETREAT)])
		Retreat_Probability(InDeltaTime);

	Reset_Probability();
}

bool UReadyFight_Widget::Show_Widget()
{
	if (Canvas)
	{
		if (R1Monster)
		{
			AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
			if (true == R1Monster->Get_LookatPlayer() && R1Monster == GameMode->CurrentPlayer->Get_CurrentPlayerState()->TargetMonster)
			{	
				Canvas->SetVisibility(ESlateVisibility::Visible);
				R1Player->End_Turn(false);

				if(ContactMonster_Sound)
					UGameplayStatics::PlaySound2D(GetWorld(), ContactMonster_Sound);
				return true;
			}

			else if (false == R1Monster->Get_LookatPlayer())
			{	
				Canvas->SetVisibility(ESlateVisibility::Hidden);
				return false;
			}
		}
	}
	return false;
}

void UReadyFight_Widget::Hovering_RandomAbilityWidget()
{
	if (bOpenWidget)
	{
		// 호버링시
		if (true == Ambush_Button->IsHovered())
			Random_Canvas->SetVisibility(ESlateVisibility::Visible);
		else if (true == Retreat_Button->IsHovered())
			Random_Canvas->SetVisibility(ESlateVisibility::Visible);
		else if (true == Fight_Button->IsHovered())
			Random_Canvas->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UReadyFight_Widget::RamdomAbility_Change(bool _result)
{
	int iCnt = { 0 };
	UImage* ResultImage = { nullptr };
	for (auto iter : RandomResult_Canvas->GetAllChildren())
	{
		UImage* uRandomImage = Cast<UImage>(iter);
		if (!uRandomImage)
			continue;

		if (RandomResult_Cnt == iCnt)
		{
			ResultImage = uRandomImage;
			break;
		}
		++iCnt;
	}
		
	if(ResultImage)
	{
		if (true == _result)
			ChangeImage(ResultImage, RandomSucess_Texture);
		else if (false == _result)
			ChangeImage(ResultImage, RandomFailed_Texture);

		++RandomResult_Cnt;
	}
}

void UReadyFight_Widget::Use_Energy(bool _using)
{
	if (true == _using) // 에너지를 사용할 때 
	{
		if (Energy_Map.Num() >= ResultCnt)
		{
			TArray<UImage*> ImageKeys;
			Energy_Map.GetKeys(ImageKeys);

			if (ImageKeys.IsValidIndex(ResultCnt))
			{
				AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
				if (GameMode->CurrentPlayer)
				{
					UImage* ImageWidget = ImageKeys[ResultCnt];
					if (false == Energy_Map[ImageWidget]) // 만약 에너지가 채워지지 않은 이미지라면
					{
						ChangeImage(ImageWidget, ChangeTexture);
						Energy_Map[ImageWidget] = true;
						ResultCnt++;
					}
				}

				// 만약 에너지를 다 채웠다면 에너지 채움을 중단
				if (ResultCnt >= Energy_Map.Num())
					GameMode->CurrentPlayer->Full_Energy(true);
			}
		}
	}
}

bool UReadyFight_Widget::Setting_Widget()
{
	Energy_Map.Emplace(Energy_1, false);
	Energy_Map.Emplace(Energy_2, false);
	Energy_Map.Emplace(Energy_3, false);

	if(Random_Canvas)
		Random_Canvas->SetVisibility(ESlateVisibility::Hidden);

	if (Fight_Button)
		Fight_Button->OnClicked.AddDynamic(this, &UReadyFight_Widget::OnFightButtonClicked);
	
	if (Ambush_Button)
		Ambush_Button->OnClicked.AddDynamic(this, &UReadyFight_Widget::OnAmbushButtonClicked);

	if (Retreat_Button)
		Retreat_Button->OnClicked.AddDynamic(this, &UReadyFight_Widget::OnRetreatButtonClicked);

	return true;
}

bool UReadyFight_Widget::Setting_Text()
{
	if (AR1Monster* FightMonster = R1Player->Get_CurrentPlayerState()->TargetMonster)
	{
		UTextBlock* MonsterName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("Monster_Name_Text")));
		UTextBlock* Document_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("FightReady_text")));

		if (MonsterName_Text)
			MonsterName_Text->SetText(FightMonster->Get_MonsterName());
		else
			return false;

		if (Document_Text)
			Document_Text->SetText(FightMonster->Get_MonsterDocument());
		else
			return false;
	}

	return true;
}

void UReadyFight_Widget::OnFightButtonClicked()
{
	R1Player->Change_Level(ESCENE::FIGHT_SCENE);
}

void UReadyFight_Widget::OnAmbushButtonClicked()
{
	bBehavior[static_cast<int32>(READY_TYPE::AMBUSH)] = true;
}

void UReadyFight_Widget::OnRetreatButtonClicked()
{
	bBehavior[static_cast<int32>(READY_TYPE::RETREAT)] = true;
}

// 기습 : 한 번의 턴 기회가 더 주어진다
void UReadyFight_Widget::Ambush_Probability(float DeltaTime)
{
	if (true == UpdateEnergyImage(DeltaTime))
	{
		// 1. 기습 성공 여부
		bool bSuccess = CheckEnergyStates();
		if (true == bSuccess)
		{
		}
		else
		{
		}

		// 2. 다음 단계로 넘어가기
		ResultCnt = 0;
		bBehavior[static_cast<int32>(READY_TYPE::AMBUSH)] = false;

		R1Player->Change_Level(ESCENE::FIGHT_SCENE);
	}
}

// 후퇴
void UReadyFight_Widget::Retreat_Probability(float DeltaTime)
{
	if (true == UpdateEnergyImage(DeltaTime))
	{
		ResultCnt = 0;

		bool bSuccess = CheckEnergyStates();
		if (true == bSuccess)
		{
			bReadyFight = true;
		}
		else if(false == bSuccess)
		{
			R1Player->Change_Level(ESCENE::FIGHT_SCENE);
		}
	}
}

void UReadyFight_Widget::Reset_Probability()
{
	if (true == bReadyFight)
	{
		Set_ShowWidget(false);
		Random_Canvas->SetVisibility(ESlateVisibility::Hidden);
		RandomResult_Canvas->SetVisibility(ESlateVisibility::Hidden);

		bBehavior[static_cast<int32>(READY_TYPE::RETREAT)] = false;
		R1Player->Get_CurrentPlayerState()->TargetMonster = nullptr;

		if (R1Player->End_Turn(true))
			bReadyFight = false;
	}
}

/* 확률 계산 */
bool UReadyFight_Widget::UpdateEnergyImage(float DeltaTime)
{
	SaveTimer += DeltaTime;

	if (READY_TIME < SaveTimer)
	{
		if (Energy_Map.Num() >= ResultCnt)
		{
			TArray<UImage*> ImageKeys;
			Energy_Map.GetKeys(ImageKeys);

			if (ImageKeys.IsValidIndex(ResultCnt))
			{
				if (R1Player)
				{
					UImage* ImageWidget = ImageKeys[ResultCnt];
					if (false == Energy_Map[ImageWidget])
					{
						if (true == R1Player->CalculateAbility(ABILITY_TYPE::SPEED))
						{
							ChangeImage(ImageWidget, ChangeTexture);
							Energy_Map[ImageWidget] = true;

							RamdomAbility_Change(true);
						}
						else
							RamdomAbility_Change(false);

						SaveTimer = 0.f;
					}
				}
			}
			++ResultCnt;
		}
	}

	return (ResultCnt >= ENERGY_CNT) ? true : false;
}

// 에너지가 다 채워졌는 지 확인
bool UReadyFight_Widget::CheckEnergyStates()
{
	TArray<UImage*> Keys;
	Energy_Map.GetKeys(Keys);

	for (int i = 0; i < Keys.Num(); i++)
	{
		bool* ValuePtr = Energy_Map.Find(Keys[i]);
		if (ValuePtr && !(*ValuePtr))
			return false;
	}
	return true;
}
