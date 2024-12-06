

#include "Interactive_Buildings/Statue/Building_Statue.h"
#include "UI/InGame/DialogEvent_Widget.h"
#include "UI/InGame/Store/Store_Widget.h"
#include "Player/R1Player.h"

ABuilding_Statue::ABuilding_Statue()
    : Super()
{
    uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_Statue"));
    RootComponent = uStaticMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Art/Orasot_Bundle/Stylized_Landscape_5_Bioms/Global/StaticMeshes/SM_Rock_Stature.SM_Rock_Stature"));
    if (MeshAsset.Succeeded())
        uStaticMesh->SetStaticMesh(MeshAsset.Object); 

	// 머티리얼 로드
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialObj(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Art/Orasot_Bundle/StylizedForestLandscape/Material_Instances/MI_Rock_Scatter_2.MI_Rock_Scatter_2'"));
	if (MaterialObj.Succeeded())
		uStaticMesh->SetMaterial(0, MaterialObj.Object);
}

void ABuilding_Statue::BeginPlay()
{
    Super::BeginPlay();

	if (!Find_Widget())
		MSG_BOX(TEXT("Widget을 생성할 수 없습니다 : ABuilding_Statue"));
}

void ABuilding_Statue::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	Exit_Widget(Dialog_Instance);
	Show_Widget(Dialog_Instance);
}

bool ABuilding_Statue::Find_Widget()
{
	/* 1. Monster HP Widget */
	if (Dialog_Widget)
		Dialog_Instance = Cast<UDialogEvent_Widget>(CreateWidget(GetWorld(), Dialog_Widget));

	if (Dialog_Instance)
	{
		Dialog_Instance->AddToViewport();
	}
	else
		return false;
	return true;
}

void ABuilding_Statue::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (AR1Character* Player = Cast<AR1Character>(OtherActor))
		{
			bStart = true;
		}
	}
}