

#include "Store/VillageForeman/R1VillageForeman.h"
#include "UI/InGame/Mission_Widget.h"


AR1VillageForeman::AR1VillageForeman()
	: Super()
{
}

void AR1VillageForeman::BeginPlay()
{
	Npc_Type = ENPC_TYPE::VILLAGEFORMAN_NPC;

	Super::BeginPlay();
}

void AR1VillageForeman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((EMISSION_TYPE::VISIT_VILLAGEMAN == MissionWidget->Get_CurrentMission() && false == MissionWidget->Get_MissionClear())
		|| true == MissionWidget->Get_MissionClear() && EMISSION_TYPE::VISIT_VILLAGEMAN_KILLMONSTER_MISSION == MissionWidget->Get_CurrentMission())
		bTarget_QuestNpc = true;
}
