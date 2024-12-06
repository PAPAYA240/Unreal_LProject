

#include "Store/Marry/R1Marry.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Interactive_Buildings/ReconTower/R1ReconTower.h"
#include "UI/InGame/DialogEvent_Widget.h"
#include "UI/InGame/Mission_Widget.h"


AR1Marry::AR1Marry()
	: Super()
{
}
void AR1Marry::BeginPlay()
{
	Npc_Type = ENPC_TYPE::MARRY_NPC;

	Super::BeginPlay();
}

void AR1Marry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if(EMISSION_TYPE::VISIT_MARRY == MissionWidget->Get_CurrentMission())
		bTarget_QuestNpc = true;

	if (7 <= Village_Instance->Get_CurrentCnt())
	{
		R1Player->Get_CurrentPlayerState()->TargetObject = nullptr;
	}

	else if (6 <=  Village_Instance->Get_CurrentCnt())
	{
		if (R1Player)
		{
			UObject* WorldContextObject = GetWorld();
			TArray<AR1ReconTower*> ReconTower_Array = AR1Player::FindActorsByClass<AR1ReconTower>(WorldContextObject);

			for (auto& iter : ReconTower_Array)
			{
				if (EREGION_TYPE::SELISIA_TYPE == iter->Get_ReconTowerType())
				{
					// 정찰탑 접근 허용
					iter->Set_FindReconTower(true);
				}
			}
		}
	}

}
