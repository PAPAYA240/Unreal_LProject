// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyBTTaskNode_FindPatrolPos.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AI/R1AIController.h"
#include "NavigationSystem.h"

UMyBTTaskNode_FindPatrolPos::UMyBTTaskNode_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UMyBTTaskNode_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation OutPatrolPos;

	if (true == NavSystem->GetRandomPointInNavigableRadius(ControllingPawn->GetActorLocation(), SearchRadius, OUT OutPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolPosKey.SelectedKeyName, OutPatrolPos);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
