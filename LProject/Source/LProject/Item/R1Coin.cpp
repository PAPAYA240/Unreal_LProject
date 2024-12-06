// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/R1Coin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Player/R1PlayerController.h"
#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h"

#define FOLLOW_START_TIME           1.5f

AR1Coin::AR1Coin()
	: Super()
{
    PrimaryActorTick.bCanEverTick = true;

    // Sphere Component 설정
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    RootComponent = SphereComponent;

    // Item Mesh 설정
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item_Coin"));
    ItemMesh->SetupAttachment(RootComponent);
    ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 충돌 활성화
    ItemMesh->SetSimulatePhysics(false); // 물리 시뮬레이션 활성화

    ItemMesh->OnComponentBeginOverlap.AddDynamic(this, &AR1Coin::OnOverlapBegin);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemObj(TEXT("/Game/Art/Outer_Model/gems_006_gems_006.gems_006_gems_006"));
    if (ItemObj.Succeeded())
    {
        ItemMesh->SetStaticMesh(ItemObj.Object);
    }
}

void AR1Coin::CheckLanding()
{
}

void AR1Coin::BeginPlay()
{
	Super::BeginPlay();
   
    InitialSpeed = 50.f;
    LaunchSpeed = 60.f; // 적절한 발사 속도로 설정
    DropHeightOffset = 100.f;
    LaunchAngle = FMath::FRandRange(40.f, 60.f);
    XYLaunchAngle = FMath::FRandRange(-360.f, 360.f); 

    // 착지 후의 처리를 위해 바닥 위치를 확인하는 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(LandingCheckTimerHandle, this, &AR1Coin::CheckLanding, 0.1f, true);
}

void AR1Coin::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
  
    if (true == CoinDrop(DeltaTime))
       CoinFollow(DeltaTime);
}

bool AR1Coin::CoinDrop(float DeltaTime)
{
    if (Coin_Timer >= FOLLOW_START_TIME)
        return true;

    if (false == bDrop)
        return false;

    Coin_Timer += DeltaTime;
    Time += DeltaTime; // 시간 증가

    // 발사 각도를 라디안으로 변환
    float Radians = FMath::DegreesToRadians(LaunchAngle);
    float XYRadian = FMath::DegreesToRadians(XYLaunchAngle);

    // 포물선 위치 계산
    FVector NewLocation;
    NewLocation.X = InitialSpeed * Time * FMath::Cos(XYRadian) ;
    NewLocation.Y = InitialSpeed * Time * FMath::Sin(XYRadian) ;
    NewLocation.Z = (InitialSpeed * FMath::Sin(Radians) * Time) + (0.5f * Gravity * FMath::Pow(Time, 2)) + DropHeightOffset;

    SetActorLocation(DropTarget + NewLocation);
    
    float RotationFrequency = FMath::RandRange(10.0f, 20.0f);
    FRotator NewRotation = FRotator(FMath::Sin(2.f * PI * RotationFrequency) * 100.f, GetActorRotation().Yaw, GetActorRotation().Roll);
    SetActorRotation(NewRotation);

    return false;
}

void AR1Coin::CoinFollow(float DeltaTime)
{
    Coin_FollowTimer += DeltaTime;

    if (Coin_FollowTimer <= 0.5f)
        return;

    if (!bFollow)
        return;
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

    APawn* PlayerPawn = PlayerController->GetPawn();
    AR1Player* R1Player = Cast<AR1Player>(PlayerPawn);

    FVector CurrentCoinPos = GetActorLocation();
    FVector TargetPos = R1Player->GetActorLocation();

    // 현재 위치와 목표 위치 간의 거리 계산
    float Distance = FVector::Dist(CurrentCoinPos, TargetPos);

    if (Distance > 1.f)
    {
        // 목표 위치로 이동
        FVector NewPosition = FMath::VInterpTo(CurrentCoinPos, TargetPos, GetWorld()->GetDeltaSeconds(), 5.0f); // 5.0f는 속도 조절
        SetActorLocation(NewPosition);
    }
}

void AR1Coin::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        AR1Player* Player = Cast<AR1Player>(OtherActor);
        if (Player)
        {
           AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
           AMyPlayerState* CurrentPlayerState = {};
           for (int i = 0; i < LAST_PLAYER; i++)
           {
               if (Player == GameMode->CurrentPlayer)
                   CurrentPlayerState = GameMode->CurrentPlayer->Get_CurrentPlayerState();
           }

           if(CurrentPlayerState)
           {
               CurrentPlayerState->TotalCoins += 1;
               Time = 0;
               bDrop = false;
               Destroy();
               return;
           }
        }
        else
        {
            if (OtherComp)
            {
            }
        }
    }

}
