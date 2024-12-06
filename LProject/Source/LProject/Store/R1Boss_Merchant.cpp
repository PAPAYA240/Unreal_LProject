// Fill out your copyright notice in the Description page of Project Settings.


#include "Store/R1Boss_Merchant.h"
#include "Camera/CameraManager.h"

#include "UI/R1UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"

#include "UI/InGame/Store/Store_Widget.h"

AR1Boss_Merchant::AR1Boss_Merchant()
	: Super()
{
}

void AR1Boss_Merchant::BeginPlay()
{
    Super::BeginPlay();

    if (!WidgetOption_Setting())
        MSG_BOX(TEXT("AR1Boss_Merchant() : Widget Option 이 세팅되지 않았습니다."));
}

void AR1Boss_Merchant::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(ShowWidget())
    {
        ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);

        if (EBOSSMAP_CAMERA::STORE_BC == MyCameraManager->Get_BCCameraIndex())
        {
            if (false == bStoreOpen)
            {
                if (ESlateVisibility::Hidden == BossMapStore_Instance->Get_ShowWidget())
                    BossMapStore_Instance->Set_ShowWidget(true);
            }
            else if (true == bStoreOpen)
            {
                if (ESlateVisibility::Visible == BossMapStore_Instance->Get_ShowWidget())
                    BossMapStore_Instance->Set_ShowWidget(false);
            }
        }
    }
    if(false == bDead)
        DestroyMerchant();
}

void AR1Boss_Merchant::DestroyMerchant()
{
    if(true == bReadyStart)
    {
        AfterStart_Timer += GetWorld()->GetDeltaSeconds();

        if(2.f <= AfterStart_Timer)
        {
            // Widget 제거
            if (BossMapStore_Instance)
            {
                BossMapStore_Instance->RemoveFromParent();
                BossMapStore_Instance = nullptr;
            }

            if (Store_Instance)
            {
                Store_Instance->RemoveFromParent();
                Store_Instance = nullptr;
            }

            Destroy();
            bDead = true;
        }
    }
}

void AR1Boss_Merchant::OnBossStoreClicked()
{
    Store_Instance->ShowStore_Widget(ESlateVisibility::Visible);

    bStoreOpen = true;
}

void AR1Boss_Merchant::OnNextButtonClicked()
{
    Store_Canvas->SetVisibility(ESlateVisibility::Hidden);
    ReadyCanvas->SetVisibility(ESlateVisibility::Visible);
    bNext = true;
}

void AR1Boss_Merchant::OnStartClicked()
{
    ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
    if (nullptr == MyCameraManager)
        return;

    // 첫 싸움
    MyCameraManager->Set_BCCameraIndex(EBOSSMAP_CAMERA::BOSS_SCENE_BC);
    BossMapStore_Instance->Set_ShowWidget(false);

    ReadyCanvas->SetVisibility(ESlateVisibility::Hidden);
    bReadyStart = true;
}

bool AR1Boss_Merchant::ShowWidget()
{
    return (ESlateVisibility::Hidden == BossMapStore_Instance->Get_ShowWidget() && false == bStoreOpen) && false == bNext ? true : false;
}

bool AR1Boss_Merchant::WidgetOption_Setting()
{
    if (BossMapStore_Widget)
        BossMapStore_Instance = Cast<UR1UserWidget>(CreateWidget(GetWorld(), BossMapStore_Widget));
    else
        return false;

    if (Store_Instance)
    {
        BossMapStore_Instance->AddToViewport(1);
    }
    else
    {
        MSG_BOX(TEXT("Widget을 생성할 수 없습니다 : AR1Boss_Merchant"));
        return false;
    }

    ReadyCanvas = Cast<UCanvasPanel>(BossMapStore_Instance->GetWidgetFromName(TEXT("Ready_Canvas")));
    Store_Canvas = Cast<UCanvasPanel>(BossMapStore_Instance->GetWidgetFromName(TEXT("Store_Canvas")));

    UButton* Store_Button = Cast<UButton>(BossMapStore_Instance->GetWidgetFromName(TEXT("Store_Button")));
    if (Store_Button)
    {
        Store_Button->OnClicked.AddDynamic(this, &AR1Boss_Merchant::OnBossStoreClicked);
    }
    else
        return false;

    UButton* NextButton = Cast<UButton>(BossMapStore_Instance->GetWidgetFromName(TEXT("Next_Button")));
    if (NextButton)
    {
        NextButton->OnClicked.AddDynamic(this, &AR1Boss_Merchant::OnNextButtonClicked);
    }
    else
        return false;

    UButton* ReadyStart_Button = Cast<UButton>(BossMapStore_Instance->GetWidgetFromName(TEXT("ReadyStart_Button")));
    if (ReadyStart_Button)
    {
        ReadyStart_Button->OnClicked.AddDynamic(this, &AR1Boss_Merchant::OnStartClicked);
        BossMapStore_Instance->Set_ShowWidget(false);
    }
    else
        return false;

    return true;
}
