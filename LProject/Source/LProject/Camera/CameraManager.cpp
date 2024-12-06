

#include "Camera/CameraManager.h"
#include "Camera/CameraComponent.h"

#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"
#include "GameFramework/PlayerController.h"

#include "Monster/R1Monster.h"
#include "Monster/Boss/R1Monster_Boss.h"
#include "Interactive_Buildings/Interactive_Buildings.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"

#include "Camera/CameraActor.h"

#include "Interactive_Buildings/PlayerSpawner/Building_PlayerSpawner.h"
#define CAMERAMOVE_SPEED        4.f

ACameraManager::ACameraManager()
	: Super()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACameraManager::BeginPlay()
{
    Super::BeginPlay();
}

bool ACameraManager::Find_Camera()
{
    if (CurrentPlayerState)
    {
        if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
        {
            TArray<AActor*> FoundCameras;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

            for (int32 i = 0; i < FoundCameras.Num(); i++)
                CameraScene.Emplace(FoundCameras[i]);
            return true;
        }
        else
            return true;
    }
    return false;
}

void ACameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
        R1Player = GameMode->CurrentPlayer;

    Opreate_Camera(DeltaTime);
    if(CurrentPlayerState)
    {
        if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
        {
            if (CameraScene.Num() <= 0)
            {
                if (!Find_Camera())
                    MSG_BOX(TEXT("Camera Actor를 찾을 수 없음"));
            }
        }
        else if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
        {
            if (!Find_Camera())
                MSG_BOX(TEXT("Camera Actor를 찾을 수 없음"));
        }
    }
}

bool ACameraManager::CameraZoomIn(float _zoomTime)
{
	if (false == bZoomIn)
	{
        ZoomIn_Timer = _zoomTime;
		bZoomIn = FocusOnObject(FOCUS_CAMERA::FOCUS_PLAYER, GetWorld()->GetDeltaSeconds());
	}

    else if (true == bZoomIn)
    {
        FVector CameraPos = CameraActor->GetComponentLocation();
		FVector PlayerPos = R1Player->GetActorLocation();
		ZoomIn_Timer -= GetWorld()->GetDeltaSeconds();

       if (ZoomIn_Timer > 0.f)
       {
           FVector NewcameraPos = FMath::VInterpTo(CameraPos, PlayerPos, GetWorld()->GetDeltaSeconds(), ZoomIn_Timer); // 속도 조절
           CameraActor->SetWorldLocation(NewcameraPos);
       }

       if (ZoomIn_Timer <= 0.f)
       {
           ZoomIn_Timer = _zoomTime;
           bZoomIn = false;
           return true;
       }
    }
    return false;
}

bool ACameraManager::CameraZoomOut(float _distance)
{
	FVector CameraPos = CameraActor->GetComponentLocation();
	FVector PlayerPos = R1Player->GetActorLocation();

	// 플레이어 방향 벡터 계산
	FVector DirectionToPlayer = (PlayerPos - CameraPos).GetSafeNormal(); // 카메라에서 플레이어 방향
	FVector BackwardDirection = -DirectionToPlayer; // 플레이어를 바라보는 방향의 반대

	// 플레이어 방향으로 멀어질 거리 설정
	FVector NewCameraPos = PlayerPos + (BackwardDirection * _distance); // 플레이어 위치에서 멀어지는 위치

    // 카메라와 플레이어 간의 거리 계산
    float CurrentDistance = FVector::Dist(CameraPos, PlayerPos);

    // 원하는 거리만큼 멀어졌을 때
    if (CurrentDistance <= _distance)
	{
		// 카메라 위치를 부드럽게 이동
		FVector InterpolatedCameraPos = FMath::VInterpTo(CameraPos, NewCameraPos, GetWorld()->GetDeltaSeconds(), 4.f); // 속도 조절
		CameraActor->SetWorldLocation(InterpolatedCameraPos);

        if (20.f >= FVector::Dist(CameraPos, NewCameraPos))
            return true;
	}

	return false; 
}

// 카메라 상태 원 위치
bool ACameraManager::Initialize_CameraPos()
{
    if (false == bChange_NextCamera)
    {
        FVector TargetCamPos = CameraActor->GetComponentLocation();
        FVector NewCamPos = FMath::VInterpTo(TargetCamPos, PrevCamPos, GetWorld()->GetDeltaSeconds(), 3.f);
        CameraActor->SetWorldLocation(NewCamPos);

        if (100.f >= FVector::Dist(NewCamPos, PrevCamPos))
            bChange_NextCamera = true;
    }
    else
    {
        if (FocusOnObject(FOCUS_CAMERA::FOCUS_PLAYER, GetWorld()->GetDeltaSeconds()))
        {
            bChange_NextCamera = false;
            return true;
        }
        else
        {
            FVector CurrentCameraPos = CameraActor->GetComponentLocation();
            CurrentCameraPos.Z = Init_CamPosition.Z;
            CameraActor->SetWorldLocation(CurrentCameraPos);
        }
    }
    return false;
}

void ACameraManager::Opreate_Camera(float DeltaTime)
{
    if (!CurrentPlayerState)
    {
        if (!Find_Player())
		{
			MSG_BOX(TEXT("플레이어를 찾을 수 없습니다 : ACameraManager"));
			return;
		}
    }

    switch (CurrentPlayerState->Player_CurrentScene)
    {
    case ESCENE::LOBBY:
        //Lobby_Camera(DeltaTime);
        break;

    case ESCENE::INGAME: 
    {
        // 1. 만약 게임의 첫 시작이 아닐 때는 Play를 true로 틀어준다.
        if(ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_PrevScene && ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
            bStartGamePlay = true;

        // 2. 게임을 처음 시작했다면,
        if (false == bStartGamePlay)
        {
            GamePlayStart();
        }
        // 3. 게임 플레이 중
        else if (true == bStartGamePlay)
        {
            // Object에 초점을 잡는 중
            if (Camera_Focus())
            {
                if (CheckObject())
                    FocusOnObject(FOCUS_CAMERA::FOCUS_OBJECT, DeltaTime);
                else
                {
                    FocusOnObject(FOCUS_CAMERA::FOCUS_PLAYER, DeltaTime);
                }

                bCameraSwitching = true;
            }
            // 정찰탑을 부숴 보스탑을 내리는 중
            else if (true == bOperateTelescope)
            {

            }
            else
            {
                AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
                if (true == GameMode->PlayerArray[0]->Get_MouseMoving())
                {
                    Camera_Move(DeltaTime);
                    bCameraSwitching = true;
                }
                else
                    bCameraSwitching = false;
            }
        }
    }
    break;

    case ESCENE::FIGHT_SCENE:
        if (true == R1Player->Get_EndFight())
        {
            Fight_End(DeltaTime);
        }
        else
        {
            if(false == bHitZoom)
            {
                Fight_CameraPosition_Setting();
                Fight_InitCamera(DeltaTime);
                Fight_SwitchOrderCamera(DeltaTime);
            }
        }
        break;

    case ESCENE::BOSS_SCENE :
        // Lobby -> Boss Scene : 보스를 보여주는 컷신
        if (ESCENE::LOBBY == CurrentPlayerState->Player_PrevScene)
        {
            FirstCutScene();
        }
        else
        {
            // Boss Second Scene
            if(InBossMap_CameraChange())
            {
                BossMap_ChangeCamera(DeltaTime);
            }
            else
            {
                //보스랑 싸우기 시작할 때 
                BossMoveFrom_Camera();
            }
        }
        break;

    default:
        break;
    }
}

bool ACameraManager::Change_CameraTime(float _speed, float _dist, float _time)
{
    // 현재 카메라 위치와 목표 위치 가져오기
    FVector CurrentPos = CameraActor->GetComponentLocation();
    FVector TargetPos = CameraScene[CurrentCameraIndex]->GetActorLocation();

    // 위치 보간
    FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), _speed);
    CameraActor->SetWorldLocation(NewPos);

    // 현재 카메라 회전과 목표 회전 가져오기
    FRotator CurrentRot = CameraActor->GetComponentRotation();
    FRotator TargetRot = CameraScene[CurrentCameraIndex]->GetActorRotation();

    // 회전 보간
    FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, GetWorld()->GetDeltaSeconds(), 2.f);
    CameraActor->SetWorldRotation(NewRot);

    // 목표 위치와의 거리 체크
    if (_dist > FVector::Dist(NewPos, TargetPos))
    {
        BossCamera_Timer += GetWorld()->GetDeltaSeconds();

        CutSceneAnimStart = BossAnimStart();

        if(BossCamera_Timer >= _time)
        {
            BossCamera_Timer = 0.f;
            ++CurrentCameraIndex;
            if (CurrentCameraIndex >= CameraScene.Num())
            {
                R1Player->Change_Level(ESCENE::INGAME);
            }
            return true;
        }
    }
    return false;
}

void ACameraManager::Lobby_Camera(float DeltaTime)
{
    if (true == R1Player->Get_ShowClassInfo())
    {
        FVector CurrentPos = CameraActor->GetComponentLocation();
        FVector GoalPos = Init_CamPosition;
        GoalPos.Y += 130.f;

        FVector NewPos = FMath::VInterpTo(CurrentPos, GoalPos, DeltaTime, 10.f);
        CameraActor->SetWorldLocation(NewPos);
    }
    else
    {
        FVector CurrentPos = CameraActor->GetComponentLocation();
        FVector NewPos = FMath::VInterpTo(CurrentPos, Init_CamPosition, DeltaTime, 10.f);
        CameraActor->SetWorldLocation(NewPos);
    }
}

void ACameraManager::Camera_Move(float DeltaTime)
{
    if (CameraActor && IsValid(CameraActor))
    {
        // 현재 마우스 위치 가져오기
        FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

        // 마우스 위치와 윈도우 중앙의 차이 계산
        MousePosition -= ViewportCenter;

        FVector2D WindowSize = GetWindowSize();
        bool bCursorMove = MousePosition.X < 0.f || MousePosition.X >= WindowSize.X ||
            MousePosition.Y < 0.f || MousePosition.Y >= WindowSize.Y;

        if (bCursorMove)
        {
            CameraMove_Timer += DeltaTime;

            if (CameraMove_Timer > 0.7f)
            {
                // 카메라의 현재 위치 가져오기
                FVector CurrentCamPos = CameraActor->GetComponentLocation();

                // 새로운 카메라 위치 계산
                FVector NewCamPos = CurrentCamPos + FVector(-MousePosition.Y, MousePosition.X, 0.0f) * CAMERAMOVE_SPEED* DeltaTime ;

                // 카메라 위치 설정
                CameraActor->SetWorldLocation(NewCamPos);
            }
        }
        else
            CameraMove_Timer = 0.f;
    }
}

bool ACameraManager::FocusOnObject(FOCUS_CAMERA _actor, float DeltaTime)
{
    if(FOCUS_CAMERA::FOCUS_OBJECT == _actor)
	{
        if (nullptr == CurrentPlayerState->TargetObject)
            return false;

		FVector ObjectPos = CurrentPlayerState->TargetObject->GetActorLocation();
		FVector CameraPos = CameraActor->GetComponentLocation();
		FRotator CameraRot = CameraActor->GetComponentRotation();
		FVector CameraForward = CameraRot.Vector();

		FVector CameraRight = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::Y); // 오른쪽 방향 벡터
		FVector TargetCameraPos = ObjectPos + (CameraForward * -1100.f) + (CameraRight * -400.f);
		TargetCameraPos.Z = CameraPos.Z;

		FVector NewCameraPos = FMath::VInterpTo(CameraPos, TargetCameraPos, GetWorld()->GetDeltaSeconds(), 3.f);
		CameraActor->SetWorldLocation(NewCameraPos);

		if (50.f > FVector::Dist(CameraPos, TargetCameraPos))
		{
			CurrentPlayerState->TargetObject->Set_ShowWidget(true); 
			return true;
		}
    }

    else if (FOCUS_CAMERA::FOCUS_PLAYER == _actor)
    {
        if (nullptr == CurrentPlayerState)
            return false;

        if(ESCENE::BOSS_SCENE != CurrentPlayerState->Player_CurrentScene)
        {
            FVector ObjectPos = R1Player->GetActorLocation();
            FVector CameraPos = CameraActor->GetComponentLocation();
            FRotator CameraRot = CameraActor->GetComponentRotation();
            FVector CameraForward = CameraRot.Vector();

            FVector TargetCameraPos = ObjectPos + (CameraForward * -1100.f);
            TargetCameraPos.Z = CameraPos.Z;

            FVector NewCameraPos = FMath::VInterpTo(CameraPos, TargetCameraPos, GetWorld()->GetDeltaSeconds(), 3.f);
            CameraActor->SetWorldLocation(NewCameraPos);
            if (10.f > FVector::Dist(CameraPos, TargetCameraPos))
            {
                bCameraFocus = false;
                return true;
            }

        }
        else
        {
            FVector PlayerPos = R1Player->GetActorLocation();
            FVector CameraPos = CameraActor->GetComponentLocation();
            FVector Distance = (PlayerPos - CameraPos).GetSafeNormal();
            FRotator NewRot = FMath::RInterpTo(CameraActor->GetComponentRotation(), Distance.Rotation(), GetWorld()->GetDeltaSeconds(), 3.f);

            CameraActor->SetWorldRotation(NewRot);
        }
    }
    return false;
}

bool ACameraManager::Camera_Focus()
{
	if (true == bCameraFocus)
        return true;

    if (!CheckObject())
        return false;
    else
    {
        bCameraFocus = true;
        return true;
    }
}

bool ACameraManager::CheckObject()
{
    return (nullptr == CurrentPlayerState->TargetObject || false == CurrentPlayerState->TargetObject->Get_StartOperate()) ? false : true;
}

void ACameraManager::GamePlayStart()
{
    if (false == bInGame)
    {
        StartGameCamera_Timer += GetWorld()->GetDeltaSeconds();
        if (StartGameCamera_Timer >= 1.f)
        {
            FVector CurrentPos = CameraActor->GetComponentLocation();
            FVector TargetPos = Init_CamPosition;
            TargetPos.Z += 3000.f;

            FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 4.5f);
            FVector PlayerRot = (R1Player->GetActorLocation() - NewPos).GetSafeNormal();
            FRotator PlayerChangeRot = PlayerRot.Rotation();

            CameraActor->SetWorldLocation(NewPos);
            CameraActor->SetWorldRotation(PlayerChangeRot);

            if (20.f >= FVector::Dist(NewPos, TargetPos)|| StartGameCamera_Timer >= 20.f)
            {
                bInGame = true;
            }
        }
    }
    else if (true == bInGame)
    {
        FVector CurrentPos = CameraActor->GetComponentLocation();
        FVector TargetPos = Init_CamPosition;

        FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 4.5f);
        FVector PlayerRot = (R1Player->GetActorLocation() - NewPos).GetSafeNormal();
        FRotator PlayerChangeRot = PlayerRot.Rotation();

        CameraActor->SetWorldLocation(NewPos);
        CameraActor->SetWorldRotation(PlayerChangeRot);

        if (10.f >= FVector::Dist(NewPos, TargetPos))
        {
            if (CurrentPlayerState->TargetObject)
            {
                ABuilding_PlayerSpawner* SpawnerBuilding = Cast<ABuilding_PlayerSpawner>(CurrentPlayerState->TargetObject);
                if (SpawnerBuilding)
                    SpawnerBuilding->Operate_SpawnParticle();
            }
            bStartGamePlay = true;
        }
    }
}

bool ACameraManager::EnterBossMap_Camera()
{
    if (false == bEnterBossMap)
        bEnterBossMap = CameraZoomIn(1.5f) ? true : false;

    if(bEnterBossMap)
    {
        ShakeTimer += GetWorld()->GetDeltaSeconds();

        if (0.05f <= ShakeTimer)
        {
            FVector CurrentPos = CameraActor->GetComponentLocation();

             FVector RandomOffset = FVector(
                 FMath::RandRange(-0.2f, 0.2f) * ShakeMagnitude,
                 FMath::RandRange(-0.2f, 0.2f) * ShakeMagnitude,
                 FMath::RandRange(-0.2f, 0.2f) * ShakeMagnitude
             );

             FVector ResultPos = CurrentPos + RandomOffset;
             ResultPos.Z = CurrentPos.Z;
             CameraActor->SetWorldLocation(ResultPos);
             ShakeTimer = 0.f;
        }
        return true;
    }
    return false;
    //GetWorld()->GetTimerManager().SetTimer(ShakeTimerHandle, this, &ACameraManager::ShakeCamera, ShakeDuration, true);
}

void ACameraManager::PlayerBack_CamPos(float _distance)
{
    FVector PlayerPos = R1Player->GetActorLocation();

    CameraActor->SetWorldRotation(R1Player->GetActorRotation());
    FVector CameraPos = CameraActor->GetComponentLocation();

    FVector DistancePos = PlayerPos - CameraPos;
    DistancePos.Normalize();

    PlayerPos.X -= _distance;
    PlayerPos.Z += 300.f;

    FVector NewPos = FMath::VInterpTo(CameraPos, PlayerPos, GetWorld()->GetDeltaSeconds(), 3.f);
    CameraActor->SetWorldLocation(NewPos);
    CameraActor->SetWorldRotation(DistancePos.Rotation());
}

void ACameraManager::ShakeCamera()
{
   
}

void ACameraManager::Fight_InitCamera(float DeltaTime)
{
    if (true == bFightStart)
        return;

    if (CameraActor && IsValid(CameraActor))
    {
        fCameraChange_Timer += DeltaTime;

        if (fCameraChange_Timer >= 2.f)
        {
            FVector NewLoc = FMath::VInterpTo(CameraActor->GetComponentLocation(), CameraChangePos.TargetLocation, DeltaTime, InterpSpeed);
            FRotator NewRot = FMath::RInterpTo(CameraActor->GetComponentRotation(), CameraChangePos.TargetRotation, DeltaTime, InterpSpeed);
            CameraActor->SetWorldLocationAndRotation(NewLoc, NewRot);
            CameraActor->SetWorldLocationAndRotation(NewLoc, NewRot);/* Y,Z,X 순서*/

            if (FVector::Dist(NewLoc, CameraChangePos.TargetLocation) < 10.f)
            {
                bFightStart = true;
                fCameraChange_Timer = 0.f;
            }
        }
        else
            CameraActor->SetWorldLocationAndRotation(FVector(-450.f, -400.f, 140.f), FRotator(0.f, 95.f, 0.f));/* Y,Z,X 순서*/
    }
}

void ACameraManager::Fight_SwitchOrderCamera(float DeltaTime)
{
    if (false == bFightStart)
        return;

    if (CameraActor && IsValid(CameraActor))
    {
        FVector NewLoc = FMath::VInterpTo(CameraActor->GetComponentLocation(), CameraChangePos.TargetLocation, DeltaTime, InterpSpeed);
        FRotator NewRot = FMath::RInterpTo(CameraActor->GetComponentRotation(), CameraChangePos.TargetRotation, DeltaTime, InterpSpeed);

        CameraActor->SetWorldLocationAndRotation(NewLoc, NewRot);/* Y,Z,X 순서*/

        if (FVector::Dist(NewLoc, CameraChangePos.TargetLocation) < 10.f)
        {
             bCameraChange = true;
        }
    }
}

void ACameraManager::Fight_CameraPosition_Setting()
{
    if (EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
    {
        CameraChangePos.TargetLocation = FVector(-657.0f, -360.f, 330.f);
        CameraChangePos.TargetRotation = FRotator(-28.7f, 73.f, 1.6f);
    }

    else if (EFIGHT_AUTHORITY::PLAYER == R1Player->Get_FightAuthority())
    {
        CameraChangePos.TargetLocation = FVector(-180, -330, 320.f);
        CameraChangePos.TargetRotation = FRotator(-28.7f, 129.7f, 1.6f);/* Y,Z,X 순서*/
    }

    else if (EFIGHT_AUTHORITY::END == R1Player->Get_FightAuthority())
    {
        CameraChangePos.TargetLocation = FVector(-350.f, -275.f, 520.f);
        CameraChangePos.TargetRotation = FRotator(-51.4f, 117.8f, -10.7f);/* Y,Z,X 순서*/
    }
}

void ACameraManager::Fight_End(float DeltaTime)
{
    FVector PlayerLocation = R1Player->GetActorLocation();
    FVector MonsterLocation = CurrentPlayerState->TargetMonster->GetActorLocation();
    FVector CenterPoint = (PlayerLocation + MonsterLocation) / 2.0f;

    CameraActor->SetWorldLocation(CenterPoint);

    CameraYaw += DeltaTime * 15.0f;

    FVector CameraOffset = FVector(-400.f, 0.f, 200.f);
    FVector CameraLocation = CenterPoint + CameraOffset.RotateAngleAxis(CameraYaw, FVector(0, 0, 1));

    CameraActor->SetWorldLocation(CameraLocation);
    FRotator LookAtRotation = (CenterPoint - CameraActor->GetComponentLocation()).Rotation();
    CameraActor->SetWorldRotation(LookAtRotation);
    
    /*else if (FightEnd_Timer <= 7.f)
    {
        FVector PlayerLocation = R1Player->GetActorLocation();
        FRotator PlayerRotation = R1Player->GetActorRotation();
        PlayerRotation += FRotator(0.0f, 180.0f, 0.0f);
        FVector SettingLoc = FVector{ -100.f, 0.f, 70.f };
        PlayerLocation += SettingLoc;
        CameraActor->SetWorldLocationAndRotation(PlayerLocation, PlayerRotation);
    }
    else if (FightEnd_Timer < 10.f)
    {
        FVector PlayerLocation = CurrentPlayerState->TargetMonster->GetActorLocation();
        CameraYaw += DeltaTime * 15.0f;
        FVector CameraOffset = FVector(-180.f, 0.f, 300.f);
        FVector CameraLocation = PlayerLocation + CameraOffset.RotateAngleAxis(CameraYaw, FVector(0, 0, 1));
        FRotator LookAtRotation = (PlayerLocation - CameraActor->GetComponentLocation()).Rotation();
        CameraActor->SetWorldLocationAndRotation(CameraLocation, LookAtRotation);
        if (FightEnd_Timer > 14.f)
        {
            FightEnd_Timer = 0.f;
        }
    }*/
}

bool ACameraManager::PitchRotation_Camera(float _timer, float _speed)
{
    if (false == bLastRotationSave)
    {
        LastRotation = CameraActor->GetComponentRotation();
        bLastRotationSave = true;
    }

    RotationTimer += GetWorld()->GetDeltaSeconds();

    if(_timer >= RotationTimer)
    {
        FRotator CurrentRotation = CameraActor->GetComponentRotation();

        float TargetPitch = CurrentRotation.Pitch + -GetWorld()->GetDeltaSeconds() * _speed;
        FRotator TargetRotation = FRotator(TargetPitch, CurrentRotation.Yaw, CurrentRotation.Roll);

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 5.f);
        CameraActor->SetWorldRotation(NewRotation);
        return false;
    }
    else
    {
        FRotator CurrentRotation = CameraActor->GetComponentRotation();

        float TargetPitch = CurrentRotation.Pitch + GetWorld()->GetDeltaSeconds() * _speed;
        FRotator TargetRotation = FRotator(TargetPitch, CurrentRotation.Yaw, CurrentRotation.Roll);

        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LastRotation, GetWorld()->GetDeltaSeconds(), 5.f);
        CameraActor->SetWorldRotation(NewRotation);

        // 쿼터니언으로 변환
        float PitchDifference = FMath::Abs(NewRotation.Pitch - LastRotation.Pitch);
        if (PitchDifference < 1.f)
        {
            CameraActor->SetWorldRotation(LastRotation);
            RotationTimer = 0.f;
            bLastRotationSave = false;
            return true;
        }
    }
    return false;
}


FVector2D ACameraManager::GetWindowSize()
{
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D WindowSize;
        GEngine->GameViewport->GetViewportSize(WindowSize);
        return WindowSize;
    }
    return FVector2D::ZeroVector;
}

bool ACameraManager::Find_Player()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
        return false;

    if (PlayerController)
    {
        AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
        R1Player = GameMode->CurrentPlayer;

        if (R1Player)
        {
            // 플레이어 셋팅 먼저
            CurrentPlayerState = GameMode->CurrentPlayer->Get_CurrentPlayerState();
            if (CurrentPlayerState)
            {
                if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
                    Find_BossScene_CamPos();

                else if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene || ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
                {
                    FActorSpawnParameters SpawnParams;
                    ACameraActor* MyCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector(-564.f, -147.f, 24.f), FRotator::ZeroRotator, SpawnParams);
                    if (MyCamera)
                        PlayerController->SetViewTarget(MyCamera);
                }
            }

            AActor* CurrentViewTarget = PlayerController->GetViewTarget();
            if (CurrentViewTarget)
            {
                // 카메라 저장
                CameraActor = CurrentViewTarget->FindComponentByClass<UCameraComponent>();
                ViewportCenter = GetWindowSize() / 2.f;
                Init_CamPosition = CameraActor->GetComponentLocation();
                Init_CamPosition.Z -= 70.f;

                PrevCamPos = CameraActor->GetComponentLocation();
            }
        }
        else
            return false;
    }
    return (nullptr != R1Player && nullptr != CurrentPlayerState) ? true : false;
}

void ACameraManager::BossMap_ChangeCamera(float DeltaTime)
{
    if (false == bBCMove)
    {
        BossMap_CameraPosition_Setting();
    }

    bBCCameraMove -= DeltaTime;
    if(0.f >= bBCCameraMove)
    {
        // Location
        FVector CurrentPos = CameraActor->GetComponentLocation();
        FVector TargetPos = BC_NextSpot.TargetLocation;
        FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, 2.f);

        FRotator CurrentRot = CameraActor->GetComponentRotation();
        FRotator TargetRot = BC_NextSpot.TargetRotation;
        FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 2.f);
        CameraActor->SetWorldLocationAndRotation(NewPos, NewRot);

        if (BCDist >= FVector::Dist(NewPos, TargetPos))
        {
            bBCMove = false;
            BCamera_PrevIndex = BCamera_Index;
            bBCCameraMove = 0.f;

            if (EBOSSMAP_CAMERA::INTOWER_START_BC == BCamera_PrevIndex)
                BCamera_Index = EBOSSMAP_CAMERA::STORE_BC;

            // Scene CamPos 가 전부 비었는 지 확인
            else if (EBOSSMAP_CAMERA::BOSS_SCENE_BC == BCamera_PrevIndex)
            {
                if (BossScene_CamPos.IsEmpty())
                {
                    BCamera_Index = EBOSSMAP_CAMERA::BOSS_FIGHT_BC;
                    BCamera_PrevIndex = EBOSSMAP_CAMERA::BOSS_FIGHT_BC;

                    CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = true;
                    CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = true;
                }
            }
        }
    }
}

void ACameraManager::BossMap_CameraPosition_Setting()
{
    // 인게임 안으로 출발
    if (EBOSSMAP_CAMERA::INTOWER_START_BC == BCamera_Index)
    {
        CameraActor->SetWorldLocationAndRotation(FVector(14.f, 21080.f, 875.f), FRotator(0.f, -90.f, 0.f));

        BC_NextSpot.TargetLocation = FVector(-6.f, 1753.f, 875.f);
        BC_NextSpot.TargetRotation = FRotator(0.f, -90.f, 0.f);/* Y,Z,X 순서*/
        bBCCameraMove = 1.5f;
        BCDist = 300.f;
    }

    if (EBOSSMAP_CAMERA::STORE_BC == BCamera_Index)
    {
        //CameraActor->SetWorldLocationAndRotation(FVector(-6.f, 1753.f, 875.f), FRotator(0.f, -90.f, 0.f));
        BC_NextSpot.TargetLocation = FVector(-70.f, 829.f, 590.f);
        BC_NextSpot.TargetRotation = FRotator(-12.79f, -79.117f, 0.26f);/* Y,Z,X 순서*/
        bBCCameraMove = 0.f;
        BCDist = 1.f;
    }

    if (EBOSSMAP_CAMERA::BOSS_SCENE_BC == BCamera_Index)
    {
        // 1. Boss Fight : Player들을 시작 위치로 옮기기
        AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
        int32 iPlayerCnt = { 0 };
        for (auto iter : GameMode->PlayerArray)
        {
            if(0 == iPlayerCnt)
            {
                iter->SetActorLocation(FVector(55.f, 740.f, 7860.f));
            }

            else if(1 == iPlayerCnt)
            {
                iter->SetActorLocation(FVector(-200.f, 740.f, 7860.f));
                iter->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)] = true;
                iter->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = false;
            }

            else if (2 == iPlayerCnt)
            {
                iter->SetActorLocation(FVector(200.f, 740.f, 7860.f));
                iter->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)] = true;
                iter->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = false;
            }
            ++iPlayerCnt;
        }

        for (int i = 0; i < BossScene_CamPos.Num(); i++)
        {
            if (!BossScene_CamPos[i].TargetLocation.IsZero())
            {
                BC_NextSpot.TargetLocation = BossScene_CamPos[i].TargetLocation;
                BC_NextSpot.TargetRotation = BossScene_CamPos[i].TargetRotation;
                BossScene_CamPos.RemoveAt(i);
                BCSceneCameraIndex ++;

                if (2 == i)
                {
                    bBCCameraMove = 3.f;
                    BCDist = 1.f;
                }
                else
                {
                    bBCCameraMove = 0.f;
                    BCDist = 200.f;
                }
                break;
            }
        }
    }
    bBCMove = true;
}

void ACameraManager::BossMoveFrom_Camera()
{
    if (!CurrentPlayerState->TargetMonster)
        return;

    if(EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
    {
        BossScene_BossCam();

        if (true == bPlayerCamSetting)
            bPlayerCamSetting = false;
    }
    else 
    {
        // 1. 공격 완료 전
        if(false == bAimAttackEnd)
        {
            BossScene_PlayerCam();
        }
        // 2. 공격 완료 후
        else if (true == bAimAttackEnd)
        {
            AimTimer += GetWorld()->GetDeltaSeconds();
            if (AimTimer >= 2.f)
            {
                AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
                // 만약 마지막 Player라면 Monster(Boss) 차례로 옮긴다.
                if (GameMode->PlayerArray[LAST_PLAYER - 1] == R1Player)
                {
                    R1Player->Change_Player();
                    for(int i = 0; i < LAST_PLAYER; i++)
                    {
                        GameMode->PlayerArray[i]->Set_FightAuthority(EFIGHT_AUTHORITY::MONSTER);
                        AimTimer = 0.f;
                        bAimAttackEnd = false;
                    }
                }
                else if (R1Player->Change_Player())
                {
                    AimTimer = 0.f;
                    bAimAttackEnd = false;
                }
            }
        }
    }
}

void ACameraManager::CameraAround(AR1Monster* TargetMonster, float _aroundSpeed)
{
    FVector PlayerPos = R1Player->GetActorLocation();
    FVector MonsterPos = TargetMonster->GetActorLocation();
    FVector CameraPos = CameraActor->GetComponentLocation();

    FVector CenterLocation = (PlayerPos + MonsterPos) / 2.0f;
    FVector CurrentLocation = CameraPos;

    /* 1. Rotation */
    FVector DirectionPos = CenterLocation - CurrentLocation;
    FVector CenterNor = DirectionPos.GetSafeNormal();
    FRotator NewRotation = CenterNor.Rotation();
    FRotator NewAroundTargetRot = FMath::RInterpTo(CameraActor->GetComponentRotation(), NewRotation, GetWorld()->GetDeltaSeconds(), 4.f);
    CameraActor->SetWorldRotation(NewAroundTargetRot);

    /* 2. Location*/
    FVector Direction = CurrentLocation - CenterLocation;

    FRotator RotationDelta(0.f, _aroundSpeed * GetWorld()->GetDeltaSeconds(), 0.f);
    Direction = RotationDelta.RotateVector(Direction);

    Direction.Normalize();
    Direction *= 1500.f;

    FVector NewLocation = CenterLocation + Direction;

    FVector NewAroundTargetPos = FMath::VInterpTo(CurrentLocation, NewLocation, GetWorld()->GetDeltaSeconds(), 4.f);
    NewAroundTargetPos.Z = MonsterPos.Z + 100.f;

    CameraActor->SetWorldLocation(NewAroundTargetPos);
}

void ACameraManager::BossScene_BossCam()
{
    AR1Monster* TargetMonster = CurrentPlayerState->TargetMonster;
    // 0. 보스가 회전 중일 때 
    if (TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::MOVING)])
    {
        CameraAround(TargetMonster, 80.f);
        HiddenAttack_Timer = 0.f;
    }

    // 1. Rwput Attack일 때 흔들리며 줌인, 줌 아웃
    else if (bRwputZoom)
    {
        AR1Monster_Boss* BossMonster = Cast<AR1Monster_Boss>(CurrentPlayerState->TargetMonster);
        if (!BossMonster) return;

        FVector CameraPos = CameraActor->GetComponentLocation();

        Rwput_Timer += GetWorld()->GetDeltaSeconds();
        // 1. 줌 인
        if (Rwput_Timer < 0.05f)
        {
            FVector NewPos = FMath::VInterpTo(CameraPos, BossMonster->Get_TargetPosition(), GetWorld()->GetDeltaSeconds(), 10.f);
            CameraActor->SetWorldLocation(NewPos);
        }

        // 2. 줌 아웃
        else if (Rwput_Timer > 0.4f && Rwput_Timer < 0.6f)
        {
            FVector Distance = (BossMonster->Get_TargetPosition() - CameraPos).GetSafeNormal();
            FVector NewPos = BossMonster->Get_TargetPosition() + (-Distance * 2500.f);
            NewPos = FMath::VInterpTo(CameraPos, NewPos, GetWorld()->GetDeltaSeconds(), 13.f);
            CameraActor->SetWorldLocation(NewPos);
        }
        else if (Rwput_Timer > 0.6f)
        {
            Rwput_Timer = 0.f;
            bRwputZoom = false;
        }
    }
    
    // 3. Hidden Attack 시
    else if (TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
    {
        HiddenAttack_Timer += GetWorld()->GetDeltaSeconds();

        if (HiddenAttack_Timer < 1.5f)
        {
            FVector CameraPos = CameraActor->GetComponentLocation();
            FRotator CameraRot = CameraActor->GetComponentRotation();

            FVector Direction = CameraRot.Vector(); // 단위 벡터
            Direction.Normalize();

            FVector NewCameraPos = CameraPos - (Direction * GetWorld()->GetDeltaSeconds() * 2000.f); // 500.f는 이동 속도입니다.
            CameraActor->SetWorldLocation(NewCameraPos);
        }
        else
        {
            FVector PlayerPos = R1Player->GetActorLocation();
            PlayerPos.X -= 800.f;
            PlayerPos.Z += 1200.f;

            FVector NewPos = FMath::VInterpTo(CameraActor->GetComponentLocation(), PlayerPos, GetWorld()->GetDeltaSeconds(), 2.f);
            CameraActor->SetWorldLocation(NewPos);

            FVector PlayerFromRot = R1Player->GetActorLocation() - CameraActor->GetComponentLocation();
            PlayerFromRot.Normalize();
            CameraActor->SetWorldRotation(PlayerFromRot.Rotation());
        }
    }
    // 4. 찍어누르기 Attack 시
    else if (TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ROWPUT_ATTACK)])
    {
        AR1Monster_Boss* BossMonster = Cast<AR1Monster_Boss>(CurrentPlayerState->TargetMonster);
        if (BossMonster)
        {
            if (BossMonster->IsRowAttackEnd())
                bRwputZoom = true;
            else
                FocusOnObject(FOCUS_CAMERA::FOCUS_PLAYER, GetWorld()->GetDeltaSeconds());
        }
    }

    // 5. Default Attack 시
    else if (TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
    {
        FocusOnObject(FOCUS_CAMERA::FOCUS_PLAYER, GetWorld()->GetDeltaSeconds());
        CameraAround(TargetMonster, -180.f);
    }
}

void ACameraManager::BossScene_PlayerCam()
{
    // 1. 보스 공격 준비 세팅
    if (false == bPlayerCamSetting)
    {
        FVector RotPos = R1Player->Get_CurrentPlayerState()->TargetMonster->GetActorLocation() - R1Player->GetActorLocation();
        CameraActor->SetWorldRotation(RotPos.Rotation());
        bPlayerCamSetting = true;
    }

    if( false == bAimAttackStart)
    {
        /* 1. 플레이어 얼굴 앞 부분에 위치 */
        FVector PlayerPos = R1Player->GetActorLocation();
        PlayerPos.Z += 60.f;
        FVector ForwardVector = R1Player->GetActorForwardVector();
        FVector NewPosition = PlayerPos + (ForwardVector * 100.0f);
        CameraActor->SetWorldLocation(NewPosition);

        /* 2. 마우스 위치에 따라 카메라가 바라보는 방향이 달라짐 */
        FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
        MousePosition -= ViewportCenter;

        FVector2D WindowSize = GetWindowSize();
        const float MouseDist = 300.f;
        bool bCursorMove = MousePosition.X < (WindowSize.X / 2.f) - MouseDist || MousePosition.X >= (WindowSize.X / 2.f) + MouseDist ||
            MousePosition.Y < (WindowSize.Y / 2.f) - MouseDist || MousePosition.Y >= (WindowSize.Y / 2.f) + MouseDist;

        if (true == bCursorMove)
        {
            // Camera Rotation
            FRotator CurrentCamRot = CameraActor->GetComponentRotation();
            FRotator NewCamRot = CurrentCamRot + FRotator(-MousePosition.Y, MousePosition.X, 0.0f) * 0.03f * GetWorld()->GetDeltaSeconds();
            CameraActor->SetWorldRotation(NewCamRot);

            // BossMonster가 뷰포트 중앙에 존재하는 지 확인
            AR1Monster* targetMonster = R1Player->Get_CurrentPlayerState()->TargetMonster;
            AR1Monster_Boss* bossMonster = Cast<AR1Monster_Boss>(targetMonster);

            if (nullptr == bossMonster)
            {
                MSG_BOX(TEXT("보스와 전투 중임에도 보스를 타겟팅하고 있지 않습니다."));
                return;
            }

            if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
            {
                FVector vBossMeshLoc = bossMonster->Get_BossSkeletalMesh()->GetComponentLocation();
                FVector2D MeshScreenPosition;
                UGameplayStatics::ProjectWorldToScreen(PlayerController, vBossMeshLoc, MeshScreenPosition);

                // 1. 정확도 측정
                float DistanceFromCenter = FVector2D::Distance(MeshScreenPosition, ViewportCenter);
                const float MinDistance = 50.f;
                const float MaxDistance = 500.f;
                int32 Accuracy = 0;

                if (DistanceFromCenter <= MinDistance)
                    Accuracy = 100;
                else if (DistanceFromCenter >= MaxDistance)
                    Accuracy = 0;
                else
                    Accuracy = FMath::Clamp(static_cast<int>(100 * (1.0f - ((DistanceFromCenter - MinDistance) / (MaxDistance - MinDistance)))), 0, 100);
                bossMonster->Set_AimPercent(Accuracy);

                AimTimer += GetWorld()->GetDeltaSeconds();
                if (AimTimer >= 4.f)
                {
                    AimTimer = 0.f;
                    bAimAttackStart = true;
                }
            }
        }
    }
    else if (true == bAimAttackStart)
    {
        // 2. Fov 조정
        float CurrentFOV = CameraActor->FieldOfView;
        if(false == bFovBack)
        {
            float NewFOV = CurrentFOV + 1.f * (GetWorld()->GetDeltaSeconds() * FovSpeed);
            CameraActor->SetFieldOfView(NewFOV);
            FovSpeed -= 50.f * GetWorld()->GetDeltaSeconds();

            if (100.f <= NewFOV)
            {
                CameraActor->SetFieldOfView(250.f);
                AimTimer += GetWorld()->GetDeltaSeconds();
                if (AimTimer >= 1.f)
                {
                    AimTimer = 0.f;
                    bFovBack = true;
                    FovSpeed = 50.f;
                }
            }
        }
        else if (true == bFovBack)
        {
            float NewFOV = CurrentFOV - 1.f * (GetWorld()->GetDeltaSeconds() * 300.f);
            NewFOV = FMath::Clamp(NewFOV, 90.f, 100.f);
            CameraActor->SetFieldOfView(NewFOV);
            if (90.f >= NewFOV)
            {
                bAimAttackEnd = true;

                CameraActor->SetFieldOfView(90.f);
                R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM_ATTACK)] = true;
                bFovBack = false;
                bAimAttackStart = false;
                bPlayerCamSetting = false;
            }
        }
    }
}

void ACameraManager::Find_BossScene_CamPos()
{
    PlayerCombatCameraSpot CamPos;
    // 1 
    CamPos.TargetLocation = FVector(41.73f, 2892.6f, 9715.6f);
    CamPos.TargetRotation = FRotator(-30.9f, -90.f, 0.f); /* Y,Z,X 순서*/
    BossScene_CamPos.Emplace(CamPos);

    // 2
    CamPos.TargetLocation = FVector(-99.2f, 356.6f, 8724.f);
    CamPos.TargetRotation = FRotator(-30.9f, -90.f, 0.f); /* Y,Z,X 순서*/
    BossScene_CamPos.Emplace(CamPos);

    // 3
    CamPos.TargetLocation = FVector(41.73f, 2892.6f, 9715.6f);
    CamPos.TargetRotation = FRotator(-30.9f, -90.f, 0.f); /* Y,Z,X 순서*/
    BossScene_CamPos.Emplace(CamPos);

    // 4
    CamPos.TargetLocation = FVector(-62.2f, 3510.6f, 11954.6f);
    CamPos.TargetRotation = FRotator(-50.6f, -90.f, 0.f); /* Y,Z,X 순서*/
    BossScene_CamPos.Emplace(CamPos);
}

void ACameraManager::FirstCutScene()
{
    APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
    if (Controller && CurrentPlayerState && CameraActor)
    {
        if (false == bCameraSetting)
        {
            if (CurrentCameraIndex >= 0 && CurrentCameraIndex < CameraScene.Num())
            {
                AActor* BatchCamera = CameraScene[CurrentCameraIndex];

                if (BatchCamera)
                {
                    FVector TargetPos = BatchCamera->GetActorLocation();
                    CameraActor->SetWorldLocation(TargetPos);
                    ++CurrentCameraIndex;

                    bCameraSetting = true;
                }
            }
        }
        else
        {
            if (1 == CurrentCameraIndex)
                Change_CameraTime(0.5f, 1500.f);
            else if (2 == CurrentCameraIndex)
                Change_CameraTime(1.f, 1000.f);
            else if (3 == CurrentCameraIndex)
                Change_CameraTime(0.8f, 200.f, 2.f);
            else if (4 == CurrentCameraIndex)
            {
                CameraSpeed_Timer += GetWorld()->GetDeltaSeconds() * (0.2f);

                Change_CameraTime(CameraSpeed_Timer, 5.f, 3.f);
            }
            //if(Camera)
            // 현재 뷰포트의 크기 가져오기
            //if(false == bCameraSetting)
            //{
            //    bCameraSetting = true;
            //    FVector2D ViewportSize;
            //    GEngine->GameViewport->GetViewportSize(ViewportSize);
            //    // 종횡비 계산
            //    float AspectRatio = ViewportSize.X / ViewportSize.Y;
            //    if (CameraScene.Num() > 0)
            //    {
            //        if (CameraScene.IsValidIndex(CurrentCameraIndex))
            //        {
            //            // 카메라의 종횡비 설정
            //            ACameraActor* CurrentCamera = Cast<ACameraActor>(CameraScene[CurrentCameraIndex]);
            //            if (CurrentCamera)
            //            {
            //                UCameraComponent* CameraComponent = CurrentCamera->GetCameraComponent();
            //                if (CameraComponent)
            //                    CameraComponent->AspectRatio = AspectRatio; // 종횡비 설정
            //            }
            //            Controller->SetViewTarget(CameraScene[CurrentCameraIndex]);
            //        }
            //    }
            //CurrentCameraIndex = (CurrentCameraIndex + 1) % CameraScene.Num();
           // }
        }
    }
}

bool ACameraManager::BossAnimStart()
{
    return (3 == CurrentCameraIndex) ? true : false;
}

bool ACameraManager::InBossMap_CameraChange()
{
    return (EBOSSMAP_CAMERA::BOSS_SCENE_BC == BCamera_Index) || (BCamera_Index != BCamera_PrevIndex) ? true : false;
}
