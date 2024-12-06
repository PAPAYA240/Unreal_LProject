

#include "UI/InGame/Fight/MonsterEnergy_Widget.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Camera/CameraManager.h"
#include "Components/CapsuleComponent.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"

#define SKILL_TIME				0.5f	// Energy가 업데이트 되는 간격
#define ENERGY_CNT				4

UMonsterEnergy_Widget::UMonsterEnergy_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMonsterEnergy_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if(Canvas)
	{
		for (auto* Widget : Canvas->GetAllChildren())
		{
			UImage* widgetImage = Cast<UImage>(Widget);
			Energy_Map.Emplace(widgetImage, false);
		}
	}
}

void UMonsterEnergy_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,  InDeltaTime);

	if (R1Monster != R1Player->Get_CurrentPlayerState()->TargetMonster)
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (true == Show_Widget())
	{
		if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
		{
			if (true == bStartEnergy)
			{
				ShowWidget_Timer = 0.f;
				if (UpdateEnergyImage(InDeltaTime))
				{
					bAttackStart = true;
					bStartEnergy = false;
				}
			}
		}

		else if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
		{
			if (UpdateEnergyImage(InDeltaTime))
			{
				MSG_BOX(TEXT("aaaaa"));
				bAttackStart = true;
			}
		}
	}
}

bool UMonsterEnergy_Widget::Show_Widget()
{
	if (!Canvas)
		return false;

	//if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
	//{
	//	if (!CurrentPlayerState->TargetMonster)
	//		return false;

	//	ShowWidget_Timer += GetWorld()->GetDeltaSeconds();

	//	// 다음 공격이 시작될 때 새로
	//	if (ShowWidget_Timer >= 1.f)
	//	{
	//		// ShowWidget_Timer가 1초 이상이라면 무조건 Widget Render를 막을 거임
	//		if (ESlateVisibility::Hidden != Canvas->GetVisibility())
	//		{
	//			Canvas->SetVisibility(ESlateVisibility::Hidden);
	//			Init_EnergyImage();
	//			bAttackStart = false;

	//			return false;
	//		}
	//		else
	//			return false;
	//	}
	//}
	
	if (EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
	{
		// 보이지 않을 때 킨다.
		if(ShowWidget_Timer <= 0.f )
		{
			if (ESlateVisibility::Hidden == Canvas->GetVisibility())
				Canvas->SetVisibility(ESlateVisibility::Visible);
		}
		if (ESlateVisibility::Visible == Canvas->GetVisibility())
		{
			ShowWidget_Timer += GetWorld()->GetDeltaSeconds();
			// 다음 공격이 시작될 때 새로
			if (ShowWidget_Timer >= 0.2f)
			{
				// ShowWidget_Timer가 1초 이상이라면 무조건 Widget Render를 막을 거임
				if (ESlateVisibility::Hidden != Canvas->GetVisibility())
				{
					Canvas->SetVisibility(ESlateVisibility::Hidden);
					Init_EnergyImage();
					bAttackStart = false;
					bStartEnergy = false;
					return false;
				}
				else
					return false;
			}
		}

		return true;
	}
	else
	{
		if(ESlateVisibility::Hidden != Canvas->GetVisibility())
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
			Init_EnergyImage();
			bAttackStart = false;
		}
		return false;
	}
}

bool UMonsterEnergy_Widget::UpdateEnergyImage(float DeltaTime)
{
	if (true == bAttackStart)
		return true;

	Set_ShowWidget(true);

	SaveTimer += DeltaTime;

	if (SKILL_TIME < SaveTimer)
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
						}
						SaveTimer = 0.f;
					}
				}
			}
			++ResultCnt;
		}
	}

	return (ResultCnt >= ENERGY_CNT) ? true : false;
}

void UMonsterEnergy_Widget::Init_EnergyImage()
{
	ResultCnt = 0;

	TArray<UImage*> ImageKeys;
	Energy_Map.GetKeys(ImageKeys);

	for (UImage* ImageWidget : ImageKeys)
	{
		ChangeImage(ImageWidget, InitTexture);
		Energy_Map[ImageWidget] = false;
	}
}
