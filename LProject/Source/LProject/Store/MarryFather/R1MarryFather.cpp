

#include "Store/MarryFather/R1MarryFather.h"
#include "UI/InGame/Mission_Widget.h"

AR1MarryFather::AR1MarryFather()
	: Super()
{
}

void AR1MarryFather::BeginPlay()
{
	Npc_Type = ENPC_TYPE::MARRY_FATHER;
	Super::BeginPlay();
}

void AR1MarryFather::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EMISSION_TYPE::VISIT_MARRYFATHER == MissionWidget->Get_CurrentMission() &&
		false == MissionWidget->Get_MissionClear())
		bTarget_QuestNpc = true;
	else
		bTarget_QuestNpc = false;
}
