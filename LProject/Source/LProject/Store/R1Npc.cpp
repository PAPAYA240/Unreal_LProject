// Fill out your copyright notice in the Description page of Project Settings.


#include "Store/R1Npc.h"
#include "Components/CapsuleComponent.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "UI/InGame/Store/Store_Widget.h"
#include "Components/VerticalBox.h"

#include "Camera/CameraManager.h"
#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h"
#include "System/R1GameInstance.h"

AR1Npc::AR1Npc()
    : Super()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 교차 오버랩
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AR1Npc::OnBeginOverlap);

    /* Data 불러오기*/
    const FString AssetPath = TEXT("/Script/LProject.R1ItemData'/Game/Data/DA_ItemData.DA_ItemData'");
    UR1ItemData* ItemData = Cast<UR1ItemData>(StaticLoadObject(UR1ItemData::StaticClass(), nullptr, *AssetPath));

    if (ItemData)
    {
        for (const FItemData_Asset& ItemAsset : ItemData->AssetItems)
        {
            if (ENPC_TYPE::POTION_MERCHANT == ItemAsset.MerchantType)
            {
                FItemData_Asset* NewItem = new FItemData_Asset();

                NewItem->ItemName = ItemAsset.ItemName;
                NewItem->ItemTexture = ItemAsset.ItemTexture;
                NewItem->ItemPurchasePrice = ItemAsset.ItemPurchasePrice;
                NewItem->ItemDocument = ItemAsset.ItemDocument;
                NewItem->ItemSalePrice = ItemAsset.ItemSalePrice;
                NewItem->ItemType = ItemAsset.ItemType;
                NewItem->MerchantType = ItemAsset.MerchantType;

                ItemAssets.Emplace(NewItem);
                Npc_Type = ItemAsset.MerchantType;
            }
        }
    }
}

void AR1Npc::BeginPlay()
{
    Super::BeginPlay();
   
    if (!Find_Player())
        MSG_BOX(TEXT("AR1Npc() : 플레이어를 찾을 수 없음"));

    if (!Load_Npc())
        MSG_BOX(TEXT("AR1Npc() : Npc가 제대로 로드되지 않았습니다."));

    Find_Widget();
  
    if (Store_Instance)
    {
        for(auto& iter : ItemAssets)
            Store_Instance->AddWidgetToVerticalBox(iter);
    }
}

void AR1Npc::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
    {
        if(nullptr != Store_Instance)
        {
            if (true == bLookAtRotation)
            {
                if(bPrevLookAtRotation != bLookAtRotation)
                {
                    Store_Instance->Set_PlayerContect(true);
                    bPrevLookAtRotation = bLookAtRotation;
                }
            }
            else
            {
                Store_Instance->Set_PlayerContect(false);
                bPrevLookAtRotation = false;
            }

            if(Exit_Widget(Store_Instance))
                Store_Instance->Set_PlayerContect(false);
        }
    }
}

bool AR1Npc::Exit_Widget(UR1UserWidget* _widget)
{
    if (true == _widget->Get_Exit())
    {
        if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
            R1Player->Get_CurrentPlayerState()->TargetObject = nullptr;

        _widget->Set_ShowWidget(false);

        if (R1Player->End_Turn(true)) // : 턴이 종료되었을 때 Turn End 줌이 필요함
        {
            _widget->Set_Exit(false);
            MSG_BOX(TEXT("NCP+TURN"));
            return true;
        }
    }

    return false;
}

void AR1Npc::OnBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AR1Npc::Pattern_Function()
{
}

bool AR1Npc::Find_Player()
{
    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        R1Player = GameMode->CurrentPlayer;
        if (R1Player)
            CurrentPlayerState = R1Player->Get_CurrentPlayerState();
    }

    return (nullptr != R1Player && nullptr != CurrentPlayerState) ? true : false;
}

bool AR1Npc::Find_Widget()
{
    /* 1. Monster Info Widget*/
    if (Store_Widget)
        Store_Instance = Cast<UStore_Widget>(CreateWidget(GetWorld(), Store_Widget));
    
    if (Store_Instance)
        Store_Instance->AddToViewport(2);
    else
        return false;

    return true;
}

bool AR1Npc::Load_Npc()
{
    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
   
    // 0. 현재 InGame Scene이고 과거 Lobby Scene 일 때
    if (ESCENE::INGAME == GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene
        && (ESCENE::LOBBY == GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_PrevScene))
    {
        AR1Npc::Load_NpcInfo            _Info = {};
        _Info.bEndDialog = bEndDialog;
        _Info.bPrevLookAtRotation = bPrevLookAtRotation;
        _Info.NextCnt = DialogNextCnt;
        _Info.NpcType = Npc_Type;
        GameInstance->Load_Npcs.Emplace(_Info);
    }

    // 1. 현재 InGame Scene이고 과거 Lobby Scene이 아니었을 때
    else if (ESCENE::INGAME == GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene
        && (ESCENE::FIGHT_SCENE == GameMode->CurrentPlayer->Get_CurrentPlayerState()->Player_PrevScene))
    {
        bLoadNpc = true;
    }
    return true;
}

