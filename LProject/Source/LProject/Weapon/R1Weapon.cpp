

#include "Weapon/R1Weapon.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

#include "Monster/R1Monster.h"
#include "Player/Player_Lumberjack/R1Player_Lumberjack.h"

#include "Engine/OverlapResult.h"
#include "Engine/HitResult.h"


AR1Weapon::AR1Weapon()
    : Super()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AR1Weapon::BeginPlay()
{
	Super::BeginPlay();

    bIsOverlapping = false; // 초기화

}

void AR1Weapon::NotifyActorBeginOverlap(AActor* OtherActor)
{
}

void AR1Weapon::NotifyActorEndOverlap(AActor* OtherActor)
{
}

void AR1Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!Find_Player())
        MSG_BOX(TEXT("플레이어를 찾을 수 없음 : Weapon"));
}

void AR1Weapon::OnOverlapBegin()
{
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);

    AR1Monster* TargetMonster = {};
    FVector MonsterPos = {};

   if (CurrentPlayerState)
    {
       TargetMonster = CurrentPlayerState->TargetMonster;

        if (TargetMonster)
            MonsterPos = CurrentPlayerState->TargetMonster->GetActorLocation();
    }

    bool bResult = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        MonsterPos,
        FQuat::Identity,
        ECollisionChannel::ECC_GameTraceChannel1,
        FCollisionShape::MakeSphere(SearchRadius),
        CollisionQueryParam
    );

    // 충돌 상태 업데이트
    if (bResult && OverlapResults.Num() > 0 && TargetMonster && !bIsOverlapping)
    {
        bIsOverlapping = true; // 충돌 발생
        // 충돌 발생 시 처리할 로직 추가
    }
    else if (!bResult || OverlapResults.Num() == 0)
    {
        bIsOverlapping = false; // 충돌 종료
    }

    FColor SphereColor = (bIsOverlapping) ? FColor::Green : FColor::Red;
    DrawDebugSphere(GetWorld(), GetActorLocation(), SearchRadius, 16, SphereColor, false, 0.1f);
}

void AR1Weapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

bool AR1Weapon::Find_Player()
{
    if (R1Player)
    {
        CurrentPlayerState = R1Player->Get_CurrentPlayerState();
        return true;
    }

    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        R1Player = GameMode->CurrentPlayer;
        if(R1Player)
            CurrentPlayerState = R1Player->Get_CurrentPlayerState();
    }

    return (R1Player != nullptr && CurrentPlayerState != nullptr);
}

void AR1Weapon::PlayExplosionEffect(FVector Location)
{
   /* if (Explosion_TargetParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion_TargetParticle, Location, FRotator::ZeroRotator, true);
    }*/
}
