#include "AI/MyBTService_FindTarget.h"
#include "R1AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/HitResult.h"
#include "System/R1GameInstance.h"

#include "Player/R1Character.h"
#include "Monster/R1Monster.h"
#include "Store/R1Npc.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"
#include "AIController.h"

UMyBTService_FindTarget::UMyBTService_FindTarget()
{
	NodeName = TEXT("FindTargetService");
	Interval = 0.5f;
}

void UMyBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* LocalPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (LocalPawn == nullptr)
		return;

	UWorld* World = LocalPawn->GetWorld();
	if (World == nullptr)
		return;

	FVector Location = LocalPawn->GetActorLocation();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, LocalPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Location,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(SearchRadius),
		CollisionQueryParam
	);

	// AI 컨트롤러 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
		return; 
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(AIController->GetWorld(), 0);
	if (PlayerController == nullptr)
		return;

	Monster_Pattern(PlayerController, OverlapResults, World, LocalPawn, OwnerComp, Location, bResult);
	
	Merchant_Pattern(PlayerController, OverlapResults, World, LocalPawn, OwnerComp, Location, bResult);


}

/* 몬스터 세팅*/
bool UMyBTService_FindTarget::Monster_Pattern(
	APlayerController* PlayerController, TArray<FOverlapResult> OverlapResults, 
	UWorld* World, APawn* LocalPawn, UBehaviorTreeComponent& OwnerComp, 
	FVector Location, bool bResult)
{
	AR1Monster* monster = Cast<AR1Monster>(LocalPawn);
	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
			AR1Player* R1Character = Cast<AR1Player>(OverlapResult.GetActor());

			if (R1Character)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, R1Character);
				DrawDebugSphere(World, Location, SearchRadius, 16, FColor::Green, false, 0.2f);

				if (monster)
				{
					AR1Player* PlayerCharacter = Cast<AR1Player>(PlayerController->GetPawn());

					AMyPlayerState* CurrentPlayerState = GameMode->CurrentPlayer->Get_CurrentPlayerState();

					if (CurrentPlayerState)
					{
						if(ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
							CurrentPlayerState->TargetMonster = monster;

						if (R1Character == GameMode->CurrentPlayer)
						{
							monster->Set_LookatPlayer(true);
						}
					}

					return true;
				}
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);
	DrawDebugSphere(World, Location, SearchRadius, 16, FColor::Red, false, 0.2f);

	return false;

}
/* 상인 세팅*/
bool UMyBTService_FindTarget::Merchant_Pattern(APlayerController* PlayerController, TArray<FOverlapResult> OverlapResults, UWorld* World, APawn* LocalPawn, UBehaviorTreeComponent& OwnerComp, FVector Location, bool bResult)
{
	AR1Npc* merchant = Cast<AR1Npc>(LocalPawn);
	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
			AR1Player* R1Character = Cast<AR1Player>(OverlapResult.GetActor());

			if (R1Character)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, R1Character);
				DrawDebugSphere(World, Location, SearchRadius, 16, FColor::Green, false, 0.2f);

				if (merchant)
				{
					AR1Player* PlayerCharacter = Cast<AR1Player>(PlayerController->GetPawn());

					UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
					AMyPlayerState* CurrentPlayerState = PlayerCharacter->GetPlayerState<AMyPlayerState>();

					/*if (CurrentPlayerState)
					{
						CurrentPlayerState->TargetMonster = monster;
					}*/
					if (R1Character == GameMode->CurrentPlayer)
					{
						merchant->Set_LookatPlayer(true);
					}
				}
				return true;
			}
		}
	}
	if (merchant)
		merchant->Set_LookatPlayer(false);

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);
	DrawDebugSphere(World, Location, SearchRadius, 16, FColor::Red, false, 0.2f);

	return false;
}


