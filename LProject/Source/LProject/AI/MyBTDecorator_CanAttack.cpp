

#include "AI/MyBTDecorator_CanAttack.h"
#include "R1AIController.h"
#include "Player/R1Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UMyBTDecorator_CanAttack::UMyBTDecorator_CanAttack()
{
	NodeName = TEXT("CanAttack");
}

bool UMyBTDecorator_CanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	AR1Character* Target = Cast<AR1Character>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if (nullptr == Target)
	{
		return false;
	}
	
	bool bCanAttack = (Target->GetDistanceTo(ControllingPawn) <= SearchRange);


	return bCanAttack;
}
