

#include "UI/InGame/PlayerBar_Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"
#include "Camera/CameraManager.h"

#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h"

#include "Player/R1Player.h"
#include "System/R1GameInstance.h"

#define SHOW_WARING 1.f

UPlayerBar_Widget::UPlayerBar_Widget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UPlayerBar_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!Setting_Monsters())
        MSG_BOX(TEXT("Failed Setting : Setting_Monsters()"));

    if (!Setting_Widget())
        MSG_BOX(TEXT("Failed Setting : UPlayerBar_Widget()"));
}

void UPlayerBar_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
    {
        Operate_InGame();
        Change_PlayerLife();
    }

    else if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
    {
        Operate_FightScene();
    }
}

void UPlayerBar_Widget::OnOption_Clicked()
{
}

void UPlayerBar_Widget::OnTurnChange_Clicked()
{
    // 다음 턴 준비 중일 때만 Click이 가능함
    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (GameMode->PlayerArray[0]->Get_MouseMoving())
    {
        ++TurnCnt;
        bNextTurn = true;

        for (auto iter : Chaos_Map)
            iter->bNextTurn = true;

        // 1. 플레이어 변경
        if (!GameMode->CurrentPlayer->Change_Player())
        {
            MSG_BOX(TEXT("APlayerBar_Widget() : 변경할 플레이어를 찾을 수 없습니다."));
        }
        GameMode->PlayerArray[0]->Set_NextTurnStart();

        // 2. 마우스 움직임을 멈춘다.
        GameMode->PlayerArray[0]->Set_StopMouseMoving();
    }
    else
        bShowWaring = true;
}

void UPlayerBar_Widget::SetOpacity(UCanvasPanel* _canvas , float _opercity)
{
    for (auto* Widget : _canvas->GetAllChildren())
    {
		UImage* image = Cast<UImage>(Widget);
        UTextBlock* text = Cast<UTextBlock>(Widget);

        if (image)
            image->SetRenderOpacity(_opercity);

        else if (text)
            text->SetRenderOpacity(_opercity);
    }
}

// 경고 [턴이 끝나지 않았습니다.]
void UPlayerBar_Widget::ShowWaring()
{
    if(ESlateVisibility::Visible != Warning_Canvas->GetVisibility())
        Warning_Canvas->SetVisibility(ESlateVisibility::Visible);

    ShowWaring_Timer += GetWorld()->GetDeltaSeconds();
    if(ShowWaring_Timer >= SHOW_WARING)
	{
		OpercityValue -= GetWorld()->GetDeltaSeconds();

		SetOpacity(Warning_Canvas, OpercityValue);

		if (OpercityValue <= 0.f)
            Stop_ShowWaring();
	}
}

void UPlayerBar_Widget::Load_Chaos_Information()
{
    if(false == bLoadWidget)
    {
        bLoadWidget = true;

        // 로드 시 한 번만 할 수 있도록 제어해보자
        UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        Chaos_Map = GameInstance->Chaos_Information;
        TurnCnt = GameInstance->Turn_Cnt;

        // 1. 이미지는 포인터이기 때문에 따로 찾아주기
        if (UCanvasPanel* ChaosCollection_Canvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Chaos_Collection_Canvas"))))
        {
            int iCnt = { 0 };
            for (auto iter : ChaosCollection_Canvas->GetAllChildren())
            {
                UImage* chaosImage = Cast<UImage>(iter);
                if (chaosImage)
                {
                    Chaos_Map[iCnt]->Chaos_Image = chaosImage;
                }
                ++iCnt;
            }
        }

        // 렌더와 포지션 세팅
        if (nullptr == R1Player)
        {
            AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
            R1Player = GameMode->CurrentPlayer;
        }

        int iCnt = { 0 };
        for (auto iter : Chaos_Map)
        {
            if (true == iter->bActive)
            {
                // 몬스터를 쓰러트렸다면 카오스를 없앤다.
                if (0 == iCnt)
                {
                    if (false == R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)])
                    {
                        iter->bActive = false;
                        iter->bNextTurn = false;
                        iter->Chaos_Cnt = 12;

                        if (iter->Chaos_Image)
                            iter->Chaos_Image->SetVisibility(ESlateVisibility::Hidden);
                    }
                }
                else
                {
                    MoveImage(iter->Chaos_Image, iter->Last_Position);
                    iter->Chaos_Image->SetVisibility(ESlateVisibility::Visible);
                }
                ++iCnt;
            }
        }
    }

}

void UPlayerBar_Widget::Set_FightType(EFIGHT_AUTHORITY _type)
{
    ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
    if (MyCameraManager)
    {
        MyCameraManager->InitCamera_NextTurn(); // 다음으로 넘길 때 카메라 상태 초기화
        Fight_AuthorityType = _type;
    }
}

void UPlayerBar_Widget::Stop_ShowWaring()
{
    OpercityValue = 1.f;
    ShowWaring_Timer = 0.f;
    SetOpacity(Warning_Canvas, 1.f);

    bShowWaring = false;
    Warning_Canvas->SetVisibility(ESlateVisibility::Hidden);
}

// 처음 시작할 때 Bar 위치 내려오기
void UPlayerBar_Widget::PlayerStart_Setting()
{
    if (R1Player->Get_GamePlayStart())
    {
        for (UWidget* Child : Canvas->GetAllChildren())
        {
            if (Child)
            {
                // 1. 현재 변환 가져오기
                FWidgetTransform WidgetTransform = Child->GetRenderTransform();
                FVector2D CurrentPosition = WidgetTransform.Translation;
                FVector2D NewPos = VInterpTo2D(CurrentPosition, Canvas_InitPos, GetWorld()->GetDeltaSeconds(), 2.f);

                // 2. 새 위치로 설정
                WidgetTransform.Translation = NewPos;
                Child->SetRenderTransform(WidgetTransform); 
            }
        }
    }
}

// 턴을 시작할 때마다 카오스 옮기기
void UPlayerBar_Widget::Move_Chaos(float DeltaTime)
{
    int32 iMapCnt = { 0 };
    for (const auto& iter : Chaos_Map)
    {
        UImage* chaos_Image = iter->Chaos_Image;
        int32& index = const_cast<int32&>(iter->Chaos_Cnt);

        // 0. Hidden인 것을은 제외
        if (ESlateVisibility::Hidden == chaos_Image->GetVisibility())
        {
            iter->bNextTurn = false;
            if (AppearanceOfChaos_Turn == TurnCnt)
            {
                TurnCnt = 0;
                chaos_Image->SetVisibility(ESlateVisibility::Visible);
            }
        }

        else if(ESlateVisibility::Visible == chaos_Image->GetVisibility())
        {
            // 1. 카오스 옮기기
            iter->bActive = true;
            float fX = GetImageXPosition(index);
            FVector2D MonsterImage_Loc = GetImageLocation(chaos_Image);
            MonsterImage_Loc.X = fX - (GetImageScale(index).X / 3.f);

            Setting_NextTurn(MonsterImage_Loc, iter);
            ++iMapCnt;
        }
    }
}

void UPlayerBar_Widget::FightTurn_Manager()
{
}

// 카오스 3턴 남았을 때 [카오스가 다가오고 있습니다] 경고
void UPlayerBar_Widget::ChoasMoving_Warning()
{
    ChoasWarning_Canvas->SetVisibility(ESlateVisibility::Visible);

    ChaosWarning_Timer += GetWorld()->GetDeltaSeconds();

    for (auto iter : ChoasWarning_Canvas->GetAllChildren())
    {
        UImage* WarningImage = Cast<UImage>(iter);

        if (nullptr != WarningImage)
        {
            float CurrentOpacity = WarningImage->GetColorAndOpacity().A;
            if(true == bUpperOpacity)
            {
                CurrentOpacity += GetWorld()->GetDeltaSeconds();
                WarningImage->SetOpacity(CurrentOpacity);
            }
            else if (false == bUpperOpacity)
            {
                CurrentOpacity -= GetWorld()->GetDeltaSeconds();
                WarningImage->SetOpacity(CurrentOpacity);
            }

            // 변경 조건
            if (WarningImage->GetColorAndOpacity().A <= 0.4f)
                bUpperOpacity = true;
            else if (WarningImage->GetColorAndOpacity().A >= 0.9f)
                bUpperOpacity = false;
        }
    }

    if (ChaosWarning_Timer > 3.f)
    {
        bChaosWarning = false;
        ChoasWarning_Canvas->SetVisibility(ESlateVisibility::Hidden);
    }
}

// 카오스 턴 움직이는 함수
float UPlayerBar_Widget::GetImageXPosition(int32 Index)
{
    if (!TurnCanvas || Index < 0 || Index >= TurnCanvas->GetChildrenCount())
    {
        MSG_BOX(TEXT("X Position을 세팅할 준비가 되지 않았습니다 : UPlayerBar_Widget"));
        return -1.0f;
    }

    UWidget* Widget = TurnCanvas->GetChildAt(Index);
    UImage* Image = Cast<UImage>(Widget);

    if (Image)
    {
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image->Slot);
        if (CanvasSlot)
        {
            FVector2D Position = CanvasSlot->GetPosition();
            return Position.X;
        }
        else
            MSG_BOX(TEXT("캔버스 슬롯이 없습니다 : UPlayerBar_Widget"));
    }
    else
        MSG_BOX(TEXT("이미지가 없습니다 : UPlayerBar_Widget"));
    return -1.0f;
}

FVector2D UPlayerBar_Widget::GetImageScale(int32 Index)
{
    if (!TurnCanvas || Index < 0 || Index >= TurnCanvas->GetChildrenCount())
    {
        MSG_BOX(TEXT("X Position을 세팅할 준비가 되지 않았습니다 : UPlayerBar_Widget"));
        return FVector2D();
    }

    UWidget* Widget = TurnCanvas->GetChildAt(Index);
    UImage* Image = Cast<UImage>(Widget);

    if (Image)
    {
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image->Slot);
        if (CanvasSlot)
        {
            // 현재 위치 가져오기
            return CanvasSlot->GetSize();
        }
        else
            MSG_BOX(TEXT("캔버스 슬롯이 없습니다 : UPlayerBar_Widget"));
    }
    else
        MSG_BOX(TEXT("이미지가 없습니다 : UPlayerBar_Widget"));

    return FVector2D();
}

// 다음 턴 관리
void UPlayerBar_Widget::Setting_NextTurn(FVector2D NewLocation, FChaosInfo* _chaosInfo)
{
    if (false == _chaosInfo->bNextTurn)
        return;

    /* 다음 턴 : 카오스 움직이기 */
    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(_chaosInfo->Chaos_Image->Slot);
    if (CanvasSlot)
    {
        // 위치 설정
        FVector2D CurrentPos = CanvasSlot->GetPosition();
        FVector2D NewPos = FMath::Lerp(CurrentPos, NewLocation, GetWorld()->GetDeltaSeconds() * 7.f);
        CanvasSlot->SetPosition(NewPos);

        if (FVector2D::Distance(CurrentPos, NewLocation) < 10.f)
        {
            // 1. 카오스 관련
            _chaosInfo->bNextTurn = false;
            for (auto iter : Chaos_Map)
            {
                if (true == iter->bNextTurn)
                    break;

                bNextTurn = false;
            }
            --_chaosInfo->Chaos_Cnt; // 카오스 턴 번호 
            _chaosInfo->Last_Position = GetImagePosition(_chaosInfo->Chaos_Image); // 도착 포지션 저장

            // 2. 카오스 저장 관련 (카오스가 도착했다면) 
            if (_chaosInfo->Chaos_Cnt <= 0)
            {
                // 1. 카오스 움직임 관련
                _chaosInfo->Chaos_Image->SetVisibility(ESlateVisibility::Hidden);
                _chaosInfo->bActive = false;
                _chaosInfo->Chaos_Cnt = MonsterTurn_Cnt;

                // 2. 카오스 저장 관련
                Chaos_Array[Chaos_Cnt]->SetVisibility(ESlateVisibility::Visible);
                if (Chaos_Cnt >= Chaos_Array.Num())
                {
                    MSG_BOX(TEXT("게임 끝"));
                }
                else
                {
                    Chaos_Cnt++;
                }
            }

            // 3. 카오스 경고 (카오스가 가까이 다가왔다면) 
            if (_chaosInfo->Chaos_Cnt <= 2)
                bChaosWarning = true;
        }
    }
}

FVector2D UPlayerBar_Widget::GetImageLocation(UImage* Image)
{
    if (Image)
    {
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image->Slot);
        if (CanvasSlot)
        {
            // 슬롯에서 위치 가져오기
            return CanvasSlot->GetPosition();
        }
    }
    return FVector2D::ZeroVector; // 이미지가 없거나 슬롯이 CanvasPanel이 아닌 경우 (0, 0) 반환
}

bool UPlayerBar_Widget::Change_PlayerLife()
{
    if (nullptr == R1Player)
        return false;

    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    R1Player = GameMode->CurrentPlayer;
    int32 iCurrentDeadLine = R1Player->Get_CurrentPlayerState()->Current_DeadLine;
    if (3 <= iCurrentDeadLine)
    {
        MSG_BOX(TEXT("죽음 3개를 채워 플레이어는 죽었습니다."));
        R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)] = true;
        return true;
    }
    else
    {
        if (Last_DeadLineCnt == iCurrentDeadLine)
            return true;

        for (int i = 0; i < LAST_PLAYER; i++)
            Life_Array[i]->SetVisibility(ESlateVisibility::Hidden);

        Last_DeadLineCnt = iCurrentDeadLine;
        if (0 == iCurrentDeadLine)
            return false;

        for (int i = 0; i < iCurrentDeadLine; i++)
        {
            if (Life_Array.Num() > 0)
                Life_Array[i]->SetVisibility(ESlateVisibility::Visible);
        }
        return true;
    }
    return false;
}

bool UPlayerBar_Widget::Setting_Widget()
{
    // 1. Setting Button
    if (UButton* OptionButton = Cast<UButton>(GetWidgetFromName(TEXT("Option_Button"))))
        OptionButton->OnClicked.AddDynamic(this, &UPlayerBar_Widget::OnOption_Clicked);
    else
        return false;

    if (UButton* NextTurnButton = Cast<UButton>(GetWidgetFromName(TEXT("NextTurn_Button"))))
        NextTurnButton->OnClicked.AddDynamic(this, &UPlayerBar_Widget::OnTurnChange_Clicked);
    else
        return false;

    Warning_Canvas->SetVisibility(ESlateVisibility::Hidden);
    ChoasWarning_Canvas->SetVisibility(ESlateVisibility::Hidden);

    // 2. Array 안에 Image 저장
    if (!Setting_WidgetArray())
        return false;

    if(nullptr == CurrentPlayerState)
    {
        AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
        CurrentPlayerState = GameMode->PlayerArray[0]->Get_CurrentPlayerState();
    }
    // 3. Scene Type에 따라서 Player Bar 변환
    Show_PlayerBarType(); 

    // 4. Scene 별 구분
    if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
    {
        for (UWidget* Child : Canvas->GetAllChildren())
        {
            if (Child)
            {
                // 현재 변환 가져오기
                FWidgetTransform WidgetTransform = Child->GetRenderTransform();
                FVector2D CurrentPosition = WidgetTransform.Translation;
                Canvas_InitPos = CurrentPosition;

                // 새 위치 계산
                FVector2D NewPosition = CurrentPosition + FVector2D(0.f, -150.f);

                // 새 위치로 설정
                WidgetTransform.Translation = NewPosition; // 새 위치로 변경
                Child->SetRenderTransform(WidgetTransform); // 변환 적용
            }
            else
                return false;
        }
    }
    else if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
    {
        // Turn Image 찾기
        TArray<AR1Monster*> MonsterArray;
        UObject* WorldContextObject = GetWorld();
        MonsterArray = AR1Player::FindActorsByClass<AR1Monster>(WorldContextObject);

        UCanvasPanel* FightCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("FightTurnCanvas")));
        if (FightCanvas)
        {
            int32 MonsterCnt = { 999 }; // 플레이어 먼저
            for (auto iter : FightCanvas->GetAllChildren())
            {
                // 이미지 등록
                UImage* FightImage = Cast<UImage>(iter);
                if (FightImage)
                {
                    if (MonsterCnt >= MonsterArray.Num())
                    {
                        ChangeImage(FightImage, R1Player->Get_ProfileTexture());
                        FightSecond_Array.Emplace(R1Player);
                        MonsterCnt = 0; 
                    }
                    else
                    {
                        if (MonsterArray[MonsterCnt]->Get_ProfileTexture())
                        {
                            ChangeImage(FightImage, MonsterArray[MonsterCnt]->Get_ProfileTexture());
                            FightSecond_Array.Emplace(MonsterArray[MonsterCnt]);
                        }
                        ++MonsterCnt; 
                    }
                }
            }
        }
        else
            return false;

        if (FightSecond_Array.Num() <= 0)
            return false;
    }
    return true;
}

void UPlayerBar_Widget::MonsterDead_ChangeTurn()
{
}

bool UPlayerBar_Widget::Destory_Chaos()
{
    for (auto iter : Chaos_Map)
    {
        if (ESlateVisibility::Visible == iter->Chaos_Image->GetVisibility())
        {
            iter->Chaos_Image->SetVisibility(ESlateVisibility::Hidden);
            iter->bActive = false;
            iter->bNextTurn = false;
            iter->Chaos_Cnt = MonsterTurn_Cnt;
            return true;
        }
    }
    return false;
}

bool UPlayerBar_Widget::Setting_Monsters()
{
    return true;
}

bool UPlayerBar_Widget::Setting_WidgetArray()
{
    for (UWidget* Widget : TurnCanvas->GetAllChildren())
    {
        UImage* Image = Cast<UImage>(Widget);
        if (Image)
            TurnBox_Array.Emplace(Image);
        else
            return false;
    }

    // Life Widget
    if (UCanvasPanel* LifeCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Life_Canvas"))))
    {
        for (UWidget* Widget : LifeCanvas->GetAllChildren())
        {
            UImage* Image = Cast<UImage>(Widget);
            if (Image)
            {
                Image->SetVisibility(ESlateVisibility::Hidden);
                Life_Array.Emplace(Image);
            }
            else
                return false;
        }
    }

    // Chaos Widget
    if (UCanvasPanel* ChaosCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Chaos_Canvas"))))
    {
        for (UWidget* Widget : ChaosCanvas->GetAllChildren())
        {
            UImage* Image = Cast<UImage>(Widget);
            if (Image)
            {
                Image->SetVisibility(ESlateVisibility::Hidden);
                Chaos_Array.Emplace(Image);
            }
            else
                return false;
        }
    }

    if (UCanvasPanel* ChaosCollection_Canvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Chaos_Collection_Canvas"))))
    {
        int32 iMapCnt = { 0 };
        for (auto iter : ChaosCollection_Canvas->GetAllChildren())
        {
            UImage* chaosImage = Cast<UImage>(iter);
            if (chaosImage)
            {
                FChaosInfo* _info = new FChaosInfo{};
                _info->bNextTurn = false;
                _info->Chaos_Cnt = 12;
                _info->Chaos_Image = chaosImage;

                if(0 == iMapCnt)
                    chaosImage->SetVisibility(ESlateVisibility::Visible);
                else
                    chaosImage->SetVisibility(ESlateVisibility::Hidden);
                Chaos_Map.Emplace(_info);
            }
            ++iMapCnt;
        }

        if (Chaos_Map.Num() <= 0)
            return false;
    }
    else
        return false;
    return true;
}

// 인게임 Tick
void UPlayerBar_Widget::Operate_InGame()
{
    // 플레이어가 게임을 시작할 때 시간 초 위치 세팅
    PlayerStart_Setting();

    // 다음 턴으로 넘어가겠다
    if (true == bNextTurn)
        Move_Chaos(GetWorld()->GetDeltaSeconds());

    if (true == bShowWaring)
        ShowWaring();

    if (true == bChaosWarning)
        ChoasMoving_Warning();
}

// Fight Scene Tick
void UPlayerBar_Widget::Operate_FightScene()
{
    // 1. 싸움이 끝났다면
    if (false == bNextCharacter)
        return;

    // 2. 싸움이 끝났다면 앞 순서를 뒤로 미룬다.
    if (FightSecond_Array.Num() > 0)
    {
        AR1Character* FirstCharacter = FightSecond_Array[0];

        TArray<AR1Character*> NewArray;
        NewArray.Add(FightSecond_Array[1]);

        for (int32 i = 2; i < FightSecond_Array.Num(); i++)
        {
            NewArray.Add(FightSecond_Array[i]);
        }

        NewArray.Add(FirstCharacter);
        FightSecond_Array = NewArray;
    }

    /* 3. 이미지 재세팅*/
    UCanvasPanel* FightCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("FightTurnCanvas")));
    if (FightCanvas)
    {
        int32 Cnt = { 0 };
        for (auto iter : FightCanvas->GetAllChildren())
        {
            /* 텍스쳐를 바꾸고 => 색상을 지정해야 색이 바뀐다.*/
            UImage* CharacterImage = Cast<UImage>(iter);
            if (CharacterImage)
            {
                ChangeImage(CharacterImage, FightSecond_Array[Cnt]->Get_ProfileTexture());
                if (AR1Monster* TargetMon = Cast<AR1Monster>(FightSecond_Array[Cnt]))
                {
                    if (true == TargetMon->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
                    {
                        FLinearColor GrayColor(0.2f, 0.2f, 0.2f, 1.0f);
                        FSlateBrush NewBrush;
                        NewBrush.SetResourceObject(FightSecond_Array[Cnt]->Get_ProfileTexture());
                        NewBrush.TintColor = FSlateColor(GrayColor);
                        CharacterImage->SetBrush(NewBrush);
                    }
                }
                ++Cnt;
            }
        }
    }

    /* 4. 새로운 타겟 세팅*/
    AR1Monster* NextMonster = Cast<AR1Monster>(FightSecond_Array[0]);
    AR1Player* NextPlayer = Cast<AR1Player>(FightSecond_Array[0]);

    if(nullptr != NextMonster)
    {
        R1Player->Get_CurrentPlayerState()->TargetMonster = NextMonster;
        Fight_AuthorityType = EFIGHT_AUTHORITY::MONSTER;
    }
    else if(nullptr != NextPlayer)
    {
        Fight_AuthorityType = EFIGHT_AUTHORITY::PLAYER;
    }
    bNextCharacter = false;
}

void UPlayerBar_Widget::Show_PlayerBarType()
{
    UCanvasPanel* FightCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("FightTurnCanvas")));
    if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
    {
        // Off
        for (auto* Widget : FightCanvas->GetAllChildren())
        {
            UImage* Image = Cast<UImage>(Widget);
            if (Image)
                Image->SetVisibility(ESlateVisibility::Hidden);
        }

        // On
        for (auto* Widget : TurnCanvas->GetAllChildren())
        {
            UImage* Image = Cast<UImage>(Widget);
            if (Image)
                Image->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene ||
        ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
    {
        // Chaos Widget 삭제
        for (auto iter : Chaos_Map)
        {
            iter->bNextTurn = false;
            iter->Chaos_Cnt = 12;
            iter->Chaos_Image->SetVisibility(ESlateVisibility::Hidden);
        }
        /*for (const TPair<UImage*, int32>& Pair : Chaos_Map)
        {
            UImage* _Image = Pair.Key; 
            if(_Image)
                _Image->SetVisibility(ESlateVisibility::Hidden);
        }*/

        // Off
        for (auto* Widget : TurnCanvas->GetAllChildren())
        {
            if (Widget)
                Widget->SetVisibility(ESlateVisibility::Hidden);
        }

        // On
        for (auto* Widget : FightCanvas->GetAllChildren())
        {
            UImage* Image = Cast<UImage>(Widget);
            if (Image) 
                Image->SetVisibility(ESlateVisibility::Visible);
        }
    }
}
