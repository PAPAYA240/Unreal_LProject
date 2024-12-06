
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Define/R1Define.h"
#include "Item/R1ItemData.h"
#include "MyPlayerState.generated.h"


UCLASS()
class LPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
   
public:
    AMyPlayerState(); // 생성자 선언

public :
    UPROPERTY(BlueprintReadWrite, Category = "Player Info")
    ESCENE Player_CurrentScene = { ESCENE::LOBBY }; // 현재 씬
    ESCENE Player_PrevScene = { ESCENE::LOBBY }; // 현재 씬

    UPROPERTY(BlueprintReadWrite, Category = "Player Info")
    EJOB_TYPE Player_Job; // 현재 플레이어 직업

    TArray<class AR1Monster*>                        FightTurnType_Array; // 싸울 몬스터 목록
    TArray<FItemData_Asset*>				            ItemAssets; // 플레이어가 들고 있는 아이템 에셋
    TObjectPtr<class AR1Monster>                   TargetMonster; // 타겟 몬스터
    TArray<EMONSTER_TYPE>	                        Target_MonsterType = { EMONSTER_TYPE::END };

    TObjectPtr<class AInteractive_Buildings>        TargetObject; // 타겟 오브젝트
    TObjectPtr<class ATrap>                             TargetTrap; // 타겟 오브젝트

    bool Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::END)] = {};

    // Custom Material
    class UMaterialInterface*	 Registration_Material[static_cast<int32>(CUSTOMIZE_PLAYER::END_CUSTOM)];

    int Energy_Cnt = { 4 }; // 현재 가지고 있는 에너지 양

    int TotalCoins = { 300 };

    bool    bNextTarget = { false };
    // 마지막 플레이어 위치
    FVector LastPlayerPosition;
    // 죽음을 몇 개 가지고 있는가?
    int32 Current_DeadLine = { 0 };



};
