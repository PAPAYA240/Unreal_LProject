

#include "UI/InGame/TreasureCard_Widget.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Math/UnrealMathUtility.h" 

UTreasureCard_Widget::UTreasureCard_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UTreasureCard_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_WidgetOption())
		MSG_BOX(TEXT("UTreasureCard_Widget() : Widget Option이 세팅되지 않았습니다."));
}

void UTreasureCard_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (true == bSelectCard)
	{
        Operate_SelectCard();
	}
}

void UTreasureCard_Widget::OnClickCard()
{
    for (auto& iter : Card_Map)
    {
        // 돌리고 있는 button이 존재한다면 return
        if (Select_Button)
            return;

        UButton* _button = iter.Key;
        if (true == _button->IsHovered())
        {
            Select_Button = _button;
            SelectReward_Array.Emplace(iter.Value);


            // 만약 4개를 맞췄다면
            int32 rewardArray[4] = { 0 };
            for (auto& iter2 : SelectReward_Array)
            {
                ++rewardArray[static_cast<int32>(iter2)];
            }

            for (int i = 0; i < 4; i++)
            {
                if (4 == rewardArray[i])
                {
                    MSG_BOX(TEXT("달성"));
                    bExit = true;
                }
            }
        }

        bSelectCard = true;
    }
}

bool UTreasureCard_Widget::Setting_WidgetOption()
{
    if (UCanvasPanel* CardCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Card_Canvas"))))
    {
        for (auto iter : CardCanvas->GetAllChildren())
        {
            UButton* CardButton = Cast<UButton>(iter);

            if (CardButton)
            {
                CardButton->OnClicked.AddDynamic(this, &UTreasureCard_Widget::OnClickCard);

                OriginSize = GetButtonSize(CardButton);
                // 초기 보상 타입을 END_CARD로 설정
                ECARD_REWARD_TYPE RewardType = ECARD_REWARD_TYPE::END_CARD;
                Card_Map.Add(CardButton, RewardType);

                // CardButton의 모든 자식 이미지 숨기기
                for (UWidget* Child : CardButton->GetAllChildren())
                {
                    UImage* CardImage = Cast<UImage>(Child);
                    if (CardImage)
                    {
                        CardImage->SetVisibility(ESlateVisibility::Hidden);
                    }
                    else
                        return false; 
                }
            }
            else
                return false; 
        }
    }
    else
        return false; 

   if (!Select_RandomCardType())
    {
        MSG_BOX(TEXT("TreasureCard_Widget() : Card의 Value가 제대로 세팅되지 않았습니다."));
        return false;
    }

    return true;
}

bool UTreasureCard_Widget::Select_RandomCardType()
{
    const int32 CardTypeCount = 4; // 카드 타입 수
    const int32 SlotCountPerType = 4; // 각 카드 타입 당 슬롯 수
    ECARD_REWARD_TYPE rewardArray[CardTypeCount][SlotCountPerType];
    std::fill(&rewardArray[0][0], &rewardArray[0][0] + sizeof(rewardArray) / sizeof(ECARD_REWARD_TYPE), ECARD_REWARD_TYPE::END_CARD);

    for (auto& iter : Card_Map) 
    {
        bool bExistence = false;
        ECARD_REWARD_TYPE eCardType;

        while (!bExistence)
        {
            eCardType = static_cast<ECARD_REWARD_TYPE>(FMath::RandRange(0, CardTypeCount - 1)); 

            for (int i = 0; i < SlotCountPerType; i++)
            {
                if (rewardArray[static_cast<int>(eCardType)][i] == ECARD_REWARD_TYPE::END_CARD)
                {
                    rewardArray[static_cast<int>(eCardType)][i] = eCardType;
                    iter.Value = eCardType;
                    bExistence = true;
                    break; 
                }
            }
        }
    }

    // 마지막 확인
    for (const auto& iter : Card_Map)
    {
        if (iter.Value == ECARD_REWARD_TYPE::END_CARD)
            return false;
    }
    return true; 
}

ECARD_REWARD_TYPE UTreasureCard_Widget::GetRandomCardRewardType()
{
	int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(ECARD_REWARD_TYPE::AWESOME_CARD));

	return static_cast<ECARD_REWARD_TYPE>(RandomIndex);
}

void UTreasureCard_Widget::Operate_SelectCard()
{
    if (nullptr == Select_Button)
        return;

    if(false == bTurnUpSide)
    {
        FVector2D ButtonSize = GetButtonSize(Select_Button);
        if (ButtonSize.X >= 10.f)
        {
            FVector2D TargetSize = ButtonSize;
            TargetSize.X = 0.f;
            FVector2D NewSize = VInterpTo2D(GetButtonSize(Select_Button), TargetSize, GetWorld()->GetDeltaSeconds(), 8.f);
            SetButtonSize(Select_Button, NewSize);
        }
        else
            bTurnUpSide = true;
    }

    else if (true == bTurnUpSide)
    {
        if(false == bVisibleImage)
        {
            for(auto iter : Card_Map)
            {
                UButton* _button = iter.Key;
                if(_button)
                {
                    if(_button == Select_Button)
                    {
                        for (auto iter2 : _button->GetAllChildren())
                        {
                            UImage* _image = Cast<UImage>(iter2);
                            if (_image)
                            {
                                _image->SetVisibility(ESlateVisibility::Visible);
                                bVisibleImage = true;
                                ChangeButtonImage(_button, SelectBox_Image);

                                if (ECARD_REWARD_TYPE::BANG_CARD == iter.Value)
                                    ChangeImage(_image, Bang_Image);

                                else if (ECARD_REWARD_TYPE::SIMPLE_CARD == iter.Value)
                                    ChangeImage(_image, Simple_Image);

                                else if (ECARD_REWARD_TYPE::MEDIUM_CARD == iter.Value)
                                    ChangeImage(_image, Medium_Image);

                                else if (ECARD_REWARD_TYPE::AWESOME_CARD == iter.Value)
                                    ChangeImage(_image, Awesome_Image);
                                else
                                    ChangeImage(_image, Bang_Image);

                                FVector2D vSize = FVector2D{ 70.f, 70.f };
                                SetImageSize_Outward(_image, vSize);

                                break;
                            }
                        }
                    }
                }
            }
        }

        FVector2D NewSize = VInterpTo2D(GetButtonSize(Select_Button), OriginSize, GetWorld()->GetDeltaSeconds(), 7.f);
        SetButtonSize(Select_Button, NewSize);

        if(1.f >= FVector2D::Distance(OriginSize, NewSize))
        {
            Reset();
        }
    }
}

void UTreasureCard_Widget::Reset()
{
    Select_Button = nullptr;

    bSelectCard = false;
    bTurnUpSide = false;
    bVisibleImage = false;
}
