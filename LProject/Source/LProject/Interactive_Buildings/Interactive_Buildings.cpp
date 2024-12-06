#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Camera/CameraManager.h"
#include "UI/R1UserWidget.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

AInteractive_Buildings::AInteractive_Buildings()
    : Super()
{
    PrimaryActorTick.bCanEverTick = true;

    //CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    //RootComponent = CapsuleComp; 

    //CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); 
    //CapsuleComp->SetCollisionResponseToAllChannels(ECR_Overlap); 
}

void AInteractive_Buildings::BeginPlay()
{
	Super::BeginPlay();
   
	if (!Find_Player())
		MSG_BOX(TEXT("Player를 찾을 수 없습니다 : ABuilding_Statue"));

    if (uStaticMesh)
    {
        uStaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
        uStaticMesh->OnComponentHit.AddDynamic(this, &AInteractive_Buildings::OnHit);
    }
}

void AInteractive_Buildings::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!CurrentPlayerState)
        return;

    // : 상호작용 시 턴 초 초기화
    if(ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
    {
        if (bStart)
            R1Player->End_Turn(false);
    }
}

/* Widget을 닫을 때 초기화 */
void AInteractive_Buildings::Return_InGame()
{
    bStart = false;
    bShowWidget = false;
    CurrentPlayerState->TargetObject = nullptr;
}

void AInteractive_Buildings::Highlight()
{
    bHighlighted = true;
    if (!uStaticMesh)
        return;
    uStaticMesh->SetRenderCustomDepth(true);
    uStaticMesh->SetCustomDepthStencilValue(250);
}

void AInteractive_Buildings::UnHighlight()
{
    bHighlighted = false;
    if (!uStaticMesh)
        return;
    uStaticMesh->SetRenderCustomDepth(false);
}

// 위젯을 보여주고 있는가?
bool AInteractive_Buildings::Show_Widget()
{
    return (true == R1Player->IsInteractable() && bStart && !bShowWidget/*bShowWidget*/); 
    /* (초를 세는 중)인터렉트 가능 &&  빌딩이 가동 되었는가? && 위젯이 열렸는가?*/
}

void AInteractive_Buildings::Set_ShowWidget(bool _show)
{
    bShowWidget = _show; 
}

bool AInteractive_Buildings::Find_Player()
{
    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        R1Player = GameMode->CurrentPlayer;
        if (R1Player)
            CurrentPlayerState = R1Player->Get_CurrentPlayerState();
    }

    return (nullptr != R1Player && nullptr != CurrentPlayerState) ? true : false;
}

bool AInteractive_Buildings::Exit_Widget(UR1UserWidget* _widget)
{
	if (true == _widget->Get_Exit())
    {
        if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
            R1Player->Get_CurrentPlayerState()->TargetObject = nullptr;

        Return_InGame();
        _widget->Set_ShowWidget(false);

        if (R1Player->End_Turn(true)) // : 턴이 종료되었을 때 Turn End 줌이 필요함
        {
            _widget->Set_Exit(false);
            return true;
        }
    }

    return false;
}

// 빌딩에 부딪히면 시작할 거임
void AInteractive_Buildings::Show_Widget(UR1UserWidget* _widget)
{
    if (Show_Widget())
    {
		if (_widget)
            _widget->Set_ShowWidget(true);
    }
}

float AInteractive_Buildings::GetMeshHeight()
{
    if (!uStaticMesh)
    {
        MSG_BOX(TEXT("스태틱 메시 컴포넌트를 찾을 수 없습니다.."))
        return 0.0f;
    }

    if (!uStaticMesh->GetStaticMesh())
    {
        MSG_BOX(TEXT("스태틱 메시가 할당되지 않았습니다."))
        return 0.0f;
    }

    FBoxSphereBounds Bounds = uStaticMesh->GetStaticMesh()->GetBounds();
    float Height = Bounds.BoxExtent.Z * 2.0f;
    return Height;
}