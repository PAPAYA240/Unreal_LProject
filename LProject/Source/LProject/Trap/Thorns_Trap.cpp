

#include "Trap/Thorns_Trap.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "GameFramework/GameModeBase.h" 
#include "Kismet/GameplayStatics.h" 

#define DISTANCE_TRAP 200.f
#define DISTANCE_UP 60.f

AThorns_Trap::AThorns_Trap()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trap_AThorns"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Art/Outer_Model/trap_traps.trap_traps'"));
	if (MeshAsset.Succeeded())
	{
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
	}
}

void AThorns_Trap::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorScale3D(FVector(5.f, 5.f, 5.f));
}

bool AThorns_Trap::PlayerInTrap()
{
	FVector PlayerOrigin, PlayerBoxExtent;
	R1Player->GetActorBounds(true, PlayerOrigin, PlayerBoxExtent);

	FVector PlayerMin = PlayerOrigin - PlayerBoxExtent;
	FVector PlayerMax = PlayerOrigin + PlayerBoxExtent;

	FVector TrapOrigin, TrapBoxExtent;
	GetActorBounds(true, TrapOrigin, TrapBoxExtent);

	FVector TrapMin = TrapOrigin - TrapBoxExtent;
	FVector TrapMax = TrapOrigin + TrapBoxExtent;

	bool bResult = (
		TrapMin.X <= PlayerMax.X && TrapMax.X >= PlayerMin.X &&
		TrapMin.Y <= PlayerMax.Y && TrapMax.Y >= PlayerMin.Y &&
		TrapMin.Z <= PlayerMax.Z && TrapMax.Z >= PlayerMin.Z
		);

	return bResult;
}

void AThorns_Trap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!R1Player || ! CurrentPlayerState)
		return;

	if(ESCENE::INGAME == R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
	{
		if (false == bAppearTrap)
		{
			bAppearTrap = PlayerInTrap();
		}
		else if (true == bAppearTrap)
		{
			Operate_Trap(DeltaTime);
		}
	}
}

bool AThorns_Trap::Operate_Trap(float DeltaTime)
{
	if (true == bOperate_Up)
		return true;

	if(false == bOperate_Start)
	{
		// 트랩 생성 위치 잡기
		bOperate_Start = true;
		FVector PlayerLocation = R1Player->GetActorLocation();

		float RandomAngle = FMath::RandRange(0.0f, 2 * PI);
		float RandomDistance = FMath::RandRange(0.0f, DISTANCE_TRAP);

		FVector NewLocation;
		NewLocation.X = PlayerLocation.X + RandomDistance * FMath::Cos(RandomAngle);
		NewLocation.Y = PlayerLocation.Y + RandomDistance * FMath::Sin(RandomAngle);
		NewLocation.Z = R1Player->GetActorLocation().Z;
		NewLocation.Z -= 100.f;
		AppearTrap_Position = NewLocation;
		AppearTrap_Position.Z += DISTANCE_UP;
		this->SetActorLocation(NewLocation);
	}
	else
	{
		FVector CurrentPos = this->GetActorLocation();
		FVector NewPos = FMath::VInterpTo(CurrentPos, AppearTrap_Position, DeltaTime, 5.f);

		this->SetActorLocation(NewPos);
		if (5.f >= FVector::Dist(NewPos, AppearTrap_Position))
		{
			bOperate_Up = true;
			return true;
		}
	}
	return false;
}
