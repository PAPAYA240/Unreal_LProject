// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/Mission_Widget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Interactive_Buildings/PlayerSpawner/Building_PlayerSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "System/R1GameInstance.h"
#include "R1GameModeBase.h"

UMission_Widget::UMission_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMission_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Setting_WidgetOption())
		MSG_BOX(TEXT("UMission_Widget() : Widget Option이 제대로 세팅되지 않았습니다."));
}

void UMission_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 플레이어가 게임을 시작할 때 위치 세팅
	if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		PlayerStart_Setting(); 
		Mission_Condition();
	}
	else if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
		Setting_MissionType(EMISSION_TYPE::FIGHT_BOSS);
	}
}

bool UMission_Widget::Setting_WidgetOption()
{
	ShowWidget();
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Setting_MissionType(GameInstance->Load_Players[0].MissionType);

	if (nullptr == CurrentPlayerState)
		return false;

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (ESCENE::INGAME == GameMode->PlayerArray[0]->Get_CurrentPlayerState()->Player_CurrentScene)
	{
		for (UWidget* Child : Canvas->GetAllChildren())
		{
			if (Child)
			{
				// 현재 변환 가져오기
				FWidgetTransform WidgetTransform = Child->GetRenderTransform();
				FVector2D CurrentPosition = WidgetTransform.Translation;
				OriginPos = CurrentPosition;

				// 새 위치 계산
				FVector2D NewPosition = CurrentPosition + FVector2D(500.f, 0.f);

				// 새 위치로 설정
				WidgetTransform.Translation = NewPosition; // 새 위치로 변경
				Child->SetRenderTransform(WidgetTransform); // 변환 적용
			}
			else
				return false;
		}
	}
	return true;
}

void UMission_Widget::PlayerStart_Setting()
{
	if (nullptr == CurrentPlayerState || nullptr == CurrentPlayerState->TargetObject)
		return;

	ABuilding_PlayerSpawner* StartSpawner = Cast<ABuilding_PlayerSpawner>(CurrentPlayerState->TargetObject);
	if (!StartSpawner)
		return;

	if (true == StartSpawner->Get_DialogEnd())
	{
		for (UWidget* Child : Canvas->GetAllChildren())
		{
			if (Child)
			{
				// 현재 변환 가져오기
				FWidgetTransform WidgetTransform = Child->GetRenderTransform();
				FVector2D CurrentPosition = WidgetTransform.Translation;
				FVector2D NewPos = VInterpTo2D(CurrentPosition, OriginPos, GetWorld()->GetDeltaSeconds(), 4.f);

				// 새 위치로 설정
				WidgetTransform.Translation = NewPos; // 새 위치로 변경
				Child->SetRenderTransform(WidgetTransform); // 변환 적용

				// Sound::Mission
				if (false == bMissionStart && MissionSound)
				{
					bMissionStart = true;
					UGameplayStatics::PlaySound2D(GetWorld(), MissionSound);
				}

				if (1.f >= FVector2D::Distance(NewPos, OriginPos))
				{
					CurrentPlayerState->TargetObject = nullptr;
				}
			}
		}
	}
}

void UMission_Widget::ShowWidget()
{
	if (nullptr == CurrentPlayerState)
	{
		MSG_BOX(TEXT("Player를 찾을 수 없습니다 : UMission_Widget"));
		return;
	}

	if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		Canvas->SetVisibility(ESlateVisibility::Visible);
	}
	else if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMission_Widget::Mission_Condition()
{
	switch (CurrentMissionType)
	{
	case EMISSION_TYPE::VISIT_VILLAGEMAN:
		
		break;

	case EMISSION_TYPE::VISIT_VILLAGEMAN_KILLMONSTER_MISSION: 
	{
		// 죽은 몬스터 실시간 변경
		UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		int32 DestroyMonsterCnt = GameInstance->DestroyMonster_Number.Num();
		FString MissionText = FString::Format(TEXT("몬스터 두 마리를 잡고 이장에게 보고하세요[2/{0}]"), { DestroyMonsterCnt });
		Description_Text->SetText(FText::FromString(MissionText));

		if (2 <= DestroyMonsterCnt)
		{
			bMissionClear = true;
			FString ClearText = FString::Format(TEXT("[CREATE] 몬스터 두 마리를 잡고 이장에게 보고하세요[2/{0}] \n[NEXT] 이장에게 돌아가기"), { DestroyMonsterCnt });
			Description_Text->SetText(FText::FromString(ClearText));
		}
	}
	break;

	case EMISSION_TYPE::END_MISSION:
		break;
	}
}

void UMission_Widget::Setting_MissionType(EMISSION_TYPE _mission)
{
	bMissionClear = false;

	CurrentMissionType = _mission;

	switch (_mission)
	{
	case EMISSION_TYPE::VISIT_VILLAGEMAN :
		Main_Text->SetText(FText::FromString(TEXT("[마을 이장] 찾아가기")));
		Description_Text->SetText(FText::FromString(TEXT("이장은 카오스에 대해 잘 알고 있을 것입니다.")));
		break;

	case EMISSION_TYPE::VISIT_VILLAGEMAN_KILLMONSTER_MISSION:
		Main_Text->SetText(FText::FromString(TEXT("마을 이장의 부탁")));
		Description_Text->SetText(FText::FromString(TEXT("몬스터 두 마리를 잡고 이장에게 보고하세요[10/0]")));
		break;

	case EMISSION_TYPE::VISIT_MARRY:
		Main_Text->SetText(FText::FromString(TEXT("상점가 딸 [메리] 를 찾아가기")));
		Description_Text->SetText(FText::FromString(TEXT("메리는 매우 똑똑한 길잡이입니다.\n당신에게 매우 도움이 될 것입니다.")));
		break;

	case EMISSION_TYPE::SELISIA_TOWER_BREAK_MISSION:
		Main_Text->SetText(FText::FromString(TEXT("[셀리시아 마을] 의 정찰탑 부수기")));
		Description_Text->SetText(FText::FromString(TEXT("메리가 알려준 길을 따라 정찰탑을 부수세요.")));
		break;

	case EMISSION_TYPE::VISIT_MARRYFATHER :
		Main_Text->SetText(FText::FromString(TEXT("메리의 아빠[메모리] 를 찾아가기")));
		Description_Text->SetText(FText::FromString(TEXT("메리는 아빠와 항상 통신합니다. 좋은 물건을 줄 거예요.")));
		break;

	case EMISSION_TYPE::FIGHT_BOSS:
		Main_Text->SetText(FText::FromString(TEXT("별의 마법사를 처치하기")));
		Description_Text->SetText(FText::FromString(TEXT("별의 탑은 얼음 마법을 사용하는 별의 마법사가 존재합니다.")));
		break;

	case EMISSION_TYPE::END_MISSION :
		break;
	}
}