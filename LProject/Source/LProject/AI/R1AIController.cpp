
#include "AI/R1AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AR1AIController::AR1AIController(const FObjectInitializer& ObjectInitializer)
{
}

void AR1AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void AR1AIController::BeginPlay()
{
	Super::BeginPlay();

	FVector Dest = {0, 0, 0};

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Dest);
	MoveRequest.SetAcceptanceRadius(15.f);

	FNavPathSharedPtr NavPath;
	MoveTo(MoveRequest, OUT &NavPath);

	if (NavPath.IsValid())
	{
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
		for (const auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.0f);
		}
	}

	//UBlackboardComponent* BlackboardComp = Blackboard;
	//if (UseBlackboard(BBAsset, BlackboardComp))
	//{
	//	RunBehaviorTree(BTAsset);
	//}
}

void AR1AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
