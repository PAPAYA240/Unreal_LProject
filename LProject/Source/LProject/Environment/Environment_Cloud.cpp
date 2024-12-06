#include "Environment/Environment_Cloud.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h" 
#include "System/R1GameInstance.h"

AEnvironment_Cloud::AEnvironment_Cloud()
{
	PrimaryActorTick.bCanEverTick = true;

	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Environment_Floor"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Tower/Meshes/GeneralEnvironment/Clouds/SM_tocloud3.SM_tocloud3'"));
	if (MeshAsset.Succeeded())
	{
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
	}
}

void AEnvironment_Cloud::BeginPlay()
{
	Super::BeginPlay();

	if (!Find_Player())
		MSG_BOX(TEXT("AEnvironment_Floor() : 플레이어를 찾지 못했습니다"));
}

void AEnvironment_Cloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Find_Player())
	{
		MSG_BOX(TEXT("AEnvironment_Floor() : 플레이어를 찾지 못했습니다"));
		return;
	}

	if(true == Destroy_Judgment())
		Destroy_Cloud();
}

bool AEnvironment_Cloud::Find_Player()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
		R1Player = GameMode->CurrentPlayer;

	return (nullptr != R1Player) ? true : false;
}

void AEnvironment_Cloud::Destroy_Cloud()
{
	if (false == bDist)
		return;

	FVector CurrentScaled = this->GetActorScale3D();
	FVector TargetScaled = FVector(0.0f, 0.0f, 0.0f);
	FVector NewScaled = FMath::VInterpTo(CurrentScaled, TargetScaled, GetWorld()->GetDeltaSeconds(), 3.f);

	SetActorScale3D(NewScaled);
	if (FVector::Dist(NewScaled, TargetScaled) < KINDA_SMALL_NUMBER)
	{
		this->SetActorScale3D(TargetScaled);
		UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		GameInstance->DestroyCloud_Number.Emplace(CloudCnt);
		Destroy();
	}
}

bool AEnvironment_Cloud::Destroy_Judgment()
{
	if (true == bDist)
		return true;

	FVector CurrentPos = this->GetActorLocation();
	FVector PlayerPos = R1Player->GetActorLocation();

	if (2000.f >= FVector::DistXY(PlayerPos, CurrentPos))
	{
		bDist = true;
		return true;
	}
	return false;
}

void AEnvironment_Cloud::AroundCloud_Destroy(FVector _targetPos)
{
	FVector CurrentCloudPos = this->GetActorLocation();
	FVector TargetPos = _targetPos;

	if (1000.f >= FVector::DistXY(CurrentCloudPos, TargetPos))
		bDist = true;
}
