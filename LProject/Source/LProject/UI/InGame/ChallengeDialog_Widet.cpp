// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/ChallengeDialog_Widet.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Player/R1Player.h"

#include "System/R1EventDialogData.h"

#define READY_TIME				0.5f	// Energy가 업데이트 되는 간격
#define ENERGY_CNT				3


UChallengeDialog_Widet::UChallengeDialog_Widet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UChallengeDialog_Widet::NativeConstruct()
{
	Super::NativeConstruct();

	for (auto iter : Energy_Canvas->GetAllChildren())
	{
		UImage* Energy_Image = Cast<UImage>(iter);
		
		if(Energy_Image)
			Energy_Map.Emplace(Energy_Image, false);
	}
	if (UButton* ChallengeButton= Cast<UButton>(GetWidgetFromName(TEXT("Challenge_Button"))))
		ChallengeButton->OnClicked.AddDynamic(this, &UChallengeDialog_Widet::OnChallengeButtonClick);
}

void UChallengeDialog_Widet::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (false == (bSettingData = Find_DialogData()))
		MSG_BOX(TEXT("UChallengeDialog_Widet() : Data가 세팅되지 않았습니다."));

	if (true == bChallenge)
	{
		if (true == UpdateEnergyImage(InDeltaTime))
		{
			// 유적일 때 살펴보기 3번
			if (EBUILDING_TYPE::OBJ_RUINS == Building_Type)
			{
				if (ChallengeCnt >= 2)
				{
					R1Player->End_Turn(true);
					bChallenge = false;
					ChallengeCnt = 0.f;
					bExit = true;
				}
				else
				{
					if(1.5f <= (ChallengeTimer+=GetWorld()->GetDeltaSeconds()))
					{
						Reset_Probability();
						++ChallengeCnt;
					}
				}
			}
			else
			{
				R1Player->End_Turn(true);
				bChallenge = false;
				bExit = true;
			}
		}
	}
}

void UChallengeDialog_Widet::OnChallengeButtonClick()
{
	bChallenge = true;
}


/* 확률 계산 */
bool UChallengeDialog_Widet::UpdateEnergyImage(float DeltaTime)
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

void UChallengeDialog_Widet::Reset_Probability()
{
	SaveTimer = 0.f;
	ResultCnt = 0;
	RandomResult_Cnt = 0;
	ChallengeTimer = 0.f;

	TArray<UImage*> ImageKeys;
	Energy_Map.GetKeys(ImageKeys);

	if (ImageKeys.IsValidIndex(ResultCnt))
	{
		for(int i = 0; i < ImageKeys.Num(); i++)
		{
			UImage* ImageWidget = ImageKeys[i];
			ChangeImage(ImageWidget, InitTexture);
			Energy_Map[ImageWidget] = false;
		}
	}

	for (auto iter : RandomResult_Canvas->GetAllChildren())
	{
		UImage* uRandomImage = Cast<UImage>(iter);
		ChangeImage(uRandomImage, RandomInit_Texture);
	}
}

void UChallengeDialog_Widet::RamdomAbility_Change(bool _result)
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

	if (ResultImage)
	{
		if (true == _result)
			ChangeImage(ResultImage, RandomSucess_Texture);
		else if (false == _result)
			ChangeImage(ResultImage, RandomFailed_Texture);

		++RandomResult_Cnt;
	}

}
void UChallengeDialog_Widet::Use_Energy(bool _using)
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

bool UChallengeDialog_Widet::Find_DialogData()
{
	if (true == bSettingData)
		return true;

	if(EBUILDING_TYPE::END_BUILDING != Building_Type)
	{
		/* Dialog Data 불러오기*/
		const FString AssetPath = TEXT("/Script/LProject.R1EventDialogData'/Game/Data/PDA_EventDialogData.PDA_EventDialogData'");
		UR1EventDialogData* EventDialogData = Cast<UR1EventDialogData>(StaticLoadObject(UR1EventDialogData::StaticClass(), nullptr, *AssetPath));

		if (EventDialogData)
		{
			for (const FEventDialogData_Asset& EventDialogAsset : EventDialogData->AssetEventDialogs)
			{
				if (Building_Type == EventDialogAsset.Building_Type)
				{
					FEventDialogData_Asset* NewDialog = new FEventDialogData_Asset();

					NewDialog->BuildingDescription = EventDialogAsset.BuildingDescription;
					NewDialog->BuildingName = EventDialogAsset.BuildingName;
					NewDialog->BuildingSubDescription = EventDialogAsset.BuildingSubDescription;
					NewDialog->BuildingTexture = EventDialogAsset.BuildingTexture;
					NewDialog->Building_ButtonDescription = EventDialogAsset.Building_ButtonDescription;

					// 빌딩 이름
					if (UTextBlock* EventName_Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("EventName_Text"))))
						EventName_Text->SetText(FText::FromString(NewDialog->BuildingName));

					// 빌딩 설명
					if (UTextBlock* StoryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Story_Text"))))
						StoryText->SetText(FText::FromString(NewDialog->BuildingDescription));

					// 빌딩 서브 설명
					if (UTextBlock* DialogText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Dialog_Text"))))
						DialogText->SetText(FText::FromString(NewDialog->BuildingSubDescription));

					// 버튼 설명
					if (UTextBlock* ButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Button_Text"))))
						ButtonText->SetText(FText::FromString(NewDialog->Building_ButtonDescription));
				}
			}
		}
		return true;
	}
	return false;
}



