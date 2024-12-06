

#include "Interactive_Buildings/RandomBox/Building_RandomBox.h"
#include "Player/R1Character.h"

#include "UI/InGame/RandomBox_Widget.h"

ABuilding_RandomBox::ABuilding_RandomBox()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_RandomBox"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Tower/Meshes/Tower/Box/SM_tobox2.SM_tobox2'"));
	if (MeshAsset.Succeeded())
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
}

void ABuilding_RandomBox::BeginPlay()
{
	Super::BeginPlay();

	if (!Find_Widget())
		MSG_BOX(TEXT("ABuilding_RandomBox() : Widget을 찾을 수 없습니다."));

	Origin_Pos = this->GetActorLocation();
}

void ABuilding_RandomBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == RandomBox_Instance->Get_Exit())
		Exit_Widget(RandomBox_Instance);

	Show_Widget(RandomBox_Instance);
	
	Floundering();
}

void ABuilding_RandomBox::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (AR1Character* Player = Cast<AR1Character>(OtherActor))
		{
			bStart = true;
		}
	}
}

bool ABuilding_RandomBox::Find_Widget()
{
	// Find Widget
	if (RandomBox_Widget)
		RandomBox_Instance = Cast<URandomBox_Widget>(CreateWidget(GetWorld(), RandomBox_Widget));
	else
		return false;

	if (RandomBox_Instance)
		RandomBox_Instance->AddToViewport();
	else
		return false;
	return true;
}

void ABuilding_RandomBox::Floundering()
{
	FVector CurrentPos = this->GetActorLocation();
	FVector TargetPos = Origin_Pos;
	TargetPos.Z += 150.f;

	if (10.f >= FVector::Dist(Origin_Pos, this->GetActorLocation()))
		bFloundering = true;
	else if (10.f >= FVector::Dist(TargetPos, this->GetActorLocation()))
		bFloundering = false;

	// 업
	if (true == bFloundering)
	{
		FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 1.f);
		this->SetActorLocation(NewPos);
	}
	// 다운
	else if (false == bFloundering)
	{
		FVector NewPos = FMath::VInterpTo(CurrentPos, Origin_Pos, GetWorld()->GetDeltaSeconds(), 1.f);
		this->SetActorLocation(NewPos);
	}
}
