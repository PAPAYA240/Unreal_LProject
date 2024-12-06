
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Player/R1LoadPlayer.h"
#include "Store/R1Npc.h"
#include "Item/R1ItemData.h"
#include "Define/R1Define.h"
#include "R1GameInstance.generated.h"


UCLASS()
class LPROJECT_API UR1GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public :
	UR1GameInstance(const FObjectInitializer& ObjectInitializer);

public :
	virtual void Init() override;
	virtual void Shutdown() override;

public :
	void Set_LoadPlayer(AR1LoadPlayer::Load_PlayerInform _player);

public : /* For. Player */
	TArray<FItemData_Asset*>							ItemAssets; // 플레이어가 들고 있는 아이템 에셋

	TArray<AR1LoadPlayer::Load_PlayerInform>	Load_Players;
	TArray<int32> DestroyMonster_Number;

	TArray<int32> DestroyCloud_Number;

public : /* PFor. Widget : Player Bar */
	TArray<FChaosInfo*>									Chaos_Information; // Chaos 움직임
	int32														Turn_Cnt = { 0 };

public : /* For. NPC*/
	TArray<AR1Npc::Load_NpcInfo> Load_Npcs;
};
