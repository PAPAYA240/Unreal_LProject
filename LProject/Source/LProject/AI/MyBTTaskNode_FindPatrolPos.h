// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MyBTTaskNode_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UMyBTTaskNode_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UMyBTTaskNode_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PatrolPosKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	float SearchRadius = 1000.f;
};