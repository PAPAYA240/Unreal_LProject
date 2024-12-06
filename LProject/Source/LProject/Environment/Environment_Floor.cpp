#include "Environment/Environment_Floor.h"
#include "Environment_Floor.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Trap/Thorns_Trap.h"

#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h" 

#include "Components/BoxComponent.h"
AEnvironment_Floor::AEnvironment_Floor()
{
	PrimaryActorTick.bCanEverTick = true;
	// uStaticMesh 컴포넌트 초기화

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxMesh->SetGenerateOverlapEvents(true);
	BoxMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	BoxMesh->OnComponentBeginOverlap.AddDynamic(this, &AEnvironment_Floor::OnOverlapBegin);

	RootComponent = BoxMesh;
}
void AEnvironment_Floor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		AR1Player* Player = Cast<AR1Player>(OtherActor);
		if (Player)
		{
			Player->Set_PlayerContextLastFloor(this);
		}
	}
}

void AEnvironment_Floor::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	
	Init_Pos = this->GetActorLocation();
	if (!Find_Player())
		MSG_BOX(TEXT("AEnvironment_Floor() : 플레이어를 찾지 못했습니다"));
	

	//int32 RandomNumber = FMath::RandRange(0, 6);
	//if (RandomNumber == 0)
	//{							
	//	FVector Loc = this->GetActorLocation();
	//	Loc.Z += 100.f;
	//	UClass* PlayerBPClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/Blueprints/Trap/Trap_1.Trap_1_C'"));
	//	if (PlayerBPClass)
	//		FloorTrap = GetWorld()->SpawnActor<AThorns_Trap>(PlayerBPClass, Loc, this->GetActorRotation());
	//}
}

void AEnvironment_Floor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Find_Player())
		MSG_BOX(TEXT("AEnvironment_Floor() : 플레이어를 찾지 못했습니다"));

	if(true == bOverlap)
	{
		if (false == bOverlapSuccess)
		{
			FVector NewPos = FMath::VInterpTo(this->GetActorLocation(), Target_Pos, DeltaTime, 3.f);
			this->SetActorLocation(NewPos);
			if (20.f >= FVector::Dist(NewPos, Target_Pos))
				bOverlapSuccess = true;
		}
		else if (true == bOverlapSuccess)
		{
			FVector NewPos = FMath::VInterpTo(this->GetActorLocation(), Init_Pos, DeltaTime, 3.f);
			this->SetActorLocation(NewPos);
			if (20.f >= FVector::Dist(NewPos, Init_Pos))
				bOverlapSuccess = true;
		}
	}
}

void AEnvironment_Floor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AR1Player>(Other))
	{
		if(this != R1Player->Get_PrevFloor())
		{
			R1Player->Set_PrevFloor(this);
			bOverlapSuccess = false;
			bOverlap = true;

			if (R1Player->Get_Jumping())
			{
				Target_Pos = Init_Pos;
				Target_Pos.Z -= 130.f;
			}
			else
			{
				Target_Pos = Init_Pos;
				Target_Pos.Z -= 60.f;
			}
		}
	}
}

bool AEnvironment_Floor::Find_Player()
{
	if (R1Player)
		return true;

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
		R1Player = GameMode->CurrentPlayer;

	return (nullptr != R1Player) ? true : false;
}

void AEnvironment_Floor::Highlight()
{
	bHighlighted = true;
	if (!BoxMesh)
		return;
	BoxMesh->SetRenderCustomDepth(true);
	BoxMesh->SetCustomDepthStencilValue(250);
}

void AEnvironment_Floor::UnHighlight()
{
	bHighlighted = false;
	if (!BoxMesh)
		return;
	BoxMesh->SetRenderCustomDepth(false);
}
