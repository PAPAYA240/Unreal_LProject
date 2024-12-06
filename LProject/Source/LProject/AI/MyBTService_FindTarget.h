#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Engine/EngineTypes.h"
#include "MyBTService_FindTarget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UMyBTService_FindTarget : public UBTService
{
	GENERATED_BODY()
	
public :
	UMyBTService_FindTarget();

protected :
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private :
	bool Monster_Pattern(class APlayerController* PlayerController, TArray<FOverlapResult> OverlapResults, UWorld* World, APawn* LocalPawn, UBehaviorTreeComponent& OwnerComp, FVector Location, bool bResult);
	bool Merchant_Pattern(class APlayerController* PlayerController, TArray<FOverlapResult> OverlapResults, UWorld* World, APawn* LocalPawn, UBehaviorTreeComponent& OwnerComp, FVector Location, bool bResult);

protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SearchRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector TargetKey;
};
