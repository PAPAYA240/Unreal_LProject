#include "Trap.h"
#include "Components/CapsuleComponent.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

ATrap::ATrap()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComp;

    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 오버랩을 위한 설정
    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Overlap); // 모든 채널에 대해 오버랩 허용
}

void ATrap::BeginPlay()
{
    Super::BeginPlay();

    Find_Player();

    if (uStaticMesh)
    {
        uStaticMesh->SetCollisionProfileName(TEXT("OverlapAll")); 
        uStaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ATrap::OnOverlapBegin); 
    }
}

void ATrap::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!Find_Player())
        MSG_BOX(TEXT("Player를 찾을 수 없습니다 : ATrap"));
}

void ATrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = true;
    MSG_BOX(TEXT(" Trap에 걸 렸 다 ! "));
	CurrentPlayerState->TargetTrap = this;
}

bool ATrap::Find_Player()
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
	return (nullptr != R1Player && nullptr != CurrentPlayerState) ? true : false;
}