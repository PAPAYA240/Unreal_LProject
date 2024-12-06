// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Fight/SelectSkill_Widget.h"
#include "UI/InGame/Fight/MonsterHP_Widget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"
#include "Camera/CameraManager.h"

#define ENERGY_CNT				4
#define SKILL_TIME				0.5f	// Energy가 업데이트 되는 간격

USelectSkill_Widget::USelectSkill_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void USelectSkill_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Inventory_Button)
		Inventory_Button->OnClicked.AddDynamic(this, &USelectSkill_Widget::OnInventoryButton_Clicked);

	if (Attack_Button)
		Attack_Button->OnClicked.AddDynamic(this, &USelectSkill_Widget::OnAttackButton_Clicked);

	if (HiddenAttack_Button)
		HiddenAttack_Button->OnClicked.AddDynamic(this, &USelectSkill_Widget::OnHiddenAttackButton_Clicked);

	if (Retreat_Button)
		Retreat_Button->OnClicked.AddDynamic(this, &USelectSkill_Widget::OnRetreatButton_Clicked);

	if (Canvas)
		Canvas->SetVisibility(ESlateVisibility::Hidden);

	for (auto* Widget : Energy_Canvas->GetAllChildren())
	{
		UImage* widgetImage = Cast<UImage>(Widget);
		Energy_Map.Emplace(widgetImage, false);
	}
}

void USelectSkill_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(true == IsGameStart())
	{
		Change_Widget();
		Skill_Pattern(InDeltaTime);
		Authority_Target();
	}

	/* 만약 타겟이 몬스터라만 Hidden으로 바꾸고 몬스터 어택 시작*/
}

void USelectSkill_Widget::OnInventoryButton_Clicked()
{
	MSG_BOX(TEXT("인벤토리"));
	bButtonClick = true;
	bBehavior[static_cast<int32>(SKILL_TYPE::INVENTORY)] = true;
}

void USelectSkill_Widget::OnAttackButton_Clicked()
{
	bButtonClick = true;
	bBehavior[static_cast<int32>(SKILL_TYPE::ATTACK)] = true;
}

void USelectSkill_Widget::OnHiddenAttackButton_Clicked()
{
	bButtonClick = true;
	bBehavior[static_cast<int32>(SKILL_TYPE::HIDDEN_ATTACK)] = true;
}

void USelectSkill_Widget::OnRetreatButton_Clicked()
{
	MSG_BOX(TEXT("Retreat"));
	bButtonClick = true;
	bBehavior[static_cast<int32>(SKILL_TYPE::RETREAT)] = true;
}

// Fight 가 시작된다면 
bool USelectSkill_Widget::IsGameStart()
{
	ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
	
	if (!MyCameraManager)
		return false;

	TArray<UUserWidget*> FoundWidgets;
	UObject* WorldContextObject = GetWorld();
	FoundWidgets = AR1Player::FindWidgetsByClass<UMonsterHP_Widget>(WorldContextObject);
	return MyCameraManager->IsFight() && Cast<UMonsterHP_Widget>(FoundWidgets[0])->Get_EndOperateVS();
}

void USelectSkill_Widget::TextSetting(FText _accuracy, FText _damage, FText _typeName)
{
	if (Accuracy_Text)
		Accuracy_Text->SetText(_accuracy);

	if (Damage_Text)
		Damage_Text->SetText(_damage);

	if (TypeName_Text)
		TypeName_Text->SetText(_typeName);
}

/* 나중에 타겟 매니저 따로 만들어주기 */
void USelectSkill_Widget::Authority_Target()
{
	if (EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
	{
		if (true == bButtonClick)
			bButtonClick = false;

		if(ESlateVisibility::Hidden != Canvas->GetVisibility() && false != bButtonClick)
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (EFIGHT_AUTHORITY::PLAYER == R1Player->Get_FightAuthority())
	{
		if(false == bButtonClick)
		{
			if (ESlateVisibility::Visible != Canvas->GetVisibility())
			{
				Init_EnergyImage();
				Canvas->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void USelectSkill_Widget::Skill_Pattern(float DeltaTime)
{
	if (true == bBehavior[static_cast<int32>(SKILL_TYPE::INVENTORY)])
	{
		CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEFAULT)] = true;

		if (Canvas)
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (true == bBehavior[static_cast<int32>(SKILL_TYPE::ATTACK)])
	{
		CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)] = true;

		if (Canvas)
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
		}

		bBehavior[static_cast<int32>(SKILL_TYPE::ATTACK)] = false;
	}

	else if (true == bBehavior[static_cast<int32>(SKILL_TYPE::HIDDEN_ATTACK)])
	{
		if (UpdateEnergyImage(DeltaTime)) 
		{
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)] = true;
			if (Canvas)
			{
				Canvas->SetVisibility(ESlateVisibility::Hidden);
			}

			bBehavior[static_cast<int32>(SKILL_TYPE::HIDDEN_ATTACK)] = false;
		}
	}
	else if (true == bBehavior[static_cast<int32>(SKILL_TYPE::RETREAT)])
	{
		CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::RETREAT)] = true;
		if (Canvas)
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USelectSkill_Widget::Change_Widget()
{
	if (true == Inventory_Button->IsHovered())
	{
		TextSetting(FText::FromString(TEXT("")), FText::FromString(TEXT("")), FText::FromString(TEXT("인벤토리")));
	}
	else if (true == Attack_Button->IsHovered())
	{
		TextSetting(FText::FromString(TEXT("6")), FText::FromString(TEXT("62")), FText::FromString(TEXT("샷")));
	}
	else if (true == HiddenAttack_Button->IsHovered())
	{
		TextSetting(FText::FromString(TEXT("6")), FText::FromString(TEXT("46")), FText::FromString(TEXT("저격 샷")));
	}
	else if (true == Retreat_Button->IsHovered())
	{
		TextSetting(FText::FromString(TEXT("")), FText::FromString(TEXT("68")), FText::FromString(TEXT("도망")));
	}
}

bool USelectSkill_Widget::UpdateEnergyImage(float DeltaTime)
{
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

void USelectSkill_Widget::Init_EnergyImage()
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

void USelectSkill_Widget::Use_Energy(bool _using)
{
	if (true == _using) // 에너지를 사용할 때 
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
					if (false == Energy_Map[ImageWidget]) // 만약 에너지가 채워지지 않은 이미지라면
					{
						ChangeImage(ImageWidget, ChangeTexture);
						Energy_Map[ImageWidget] = true;
						ResultCnt++;
					}
				}

				// 만약 에너지를 다 채웠다면 에너지 채움을 중단
				if (ResultCnt >= Energy_Map.Num())
					R1Player->Full_Energy(true);
			}
		}
	}
}


