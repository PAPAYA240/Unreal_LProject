

#include "Interactive_Buildings/Object/Building_Sign.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/InGame/HintDialog_Widget.h"
#include "UI/InGame/Fight/ResultFight_Widget.h"

#include "Camera/CameraManager.h"
#include "Camera/CameraComponent.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Character.h"
#include "Environment/Environment_Cliff.h"

#include "Engine/TriggerBox.h"


ABuilding_Sign::ABuilding_Sign()
	: Super()
{
	uStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building_ObjSign"));
	RootComponent = uStaticMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Art/Orasot_Bundle/LowPolyForestVol2/StaticMeshes/Environment/SM_Env_sign_1.SM_Env_sign_1'"));
	if (MeshAsset.Succeeded())
		uStaticMesh->SetStaticMesh(MeshAsset.Object);
}

void ABuilding_Sign::BeginPlay()
{
	Super::BeginPlay();

	Building_Type = EBUILDING_TYPE::OBJ_SIGN;

	if (!Setting_WidgetOption())
		MSG_BOX(TEXT("ABuilding_Sign() : Widget Option이 세팅되지 않았습니다."));
	
}

void ABuilding_Sign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Show_Widget(Dialog_Instance);

	// Widget을 가동시켰다면 미니게임을 시작한다.
	if (false == bOperate && ESlateVisibility::Visible == Dialog_Instance->Get_ShowWidget())
		bOperate = true;

	if(true == bOperate)
		Operate_MiniGame();
}

void ABuilding_Sign::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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

void ABuilding_Sign::Return()
{
	UObject* WorldContextObject = GetWorld();
	TArray<ATriggerBox*> TriggerBoxArray = AR1Player::FindActorsByClass<ATriggerBox>(WorldContextObject);

	if (TriggerBoxArray.Num() > 0)
	{
		// 2. Cliff 초기화
		ATriggerBox* _TriggerBox = TriggerBoxArray[0];
		R1Player->SetActorLocation(_TriggerBox->GetActorLocation());

		for (auto iter : Cliff_Array)
			iter->Set_Upper_Cliff(false);
	}
}

void ABuilding_Sign::Operate_MiniGame()
{
	if (false == bFailed)
	{
		if (true == Dialog_Instance->Get_Exit())
		{
			// 절벽 찾기
			if (0 >= Cliff_Array.Num())
			{
				if (R1Player)
				{
					UObject* WorldContextObject = GetWorld();
					Cliff_Array = AR1Player::FindActorsByClass<AEnvironment_Cliff>(WorldContextObject);
				}
			}

			for (auto iter : Cliff_Array)
			{
				FVector PlayerPos = R1Player->GetActorLocation();
				FVector iterPos = iter->GetActorLocation();

				float DistXY = FVector::DistXY(PlayerPos, iterPos);

				if (500.f > DistXY)
					iter->Set_Upper_Cliff(true);
			}

			// 카메라
			Dialog_Instance->Set_ShowWidget(false);
			R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)] = true;

			if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
			{
				// Camera 조정
				MyCameraManager->PlayerBack_CamPos(400.f);
			}


			// 만약 떨어졌다면,
			if (true == R1Player->Falling_Player())
			{
				bStart = false;
				bFailed = true;
				R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)] = false;
			}
			//{
			//	// 1. widget 불러오기
			//	TArray<UUserWidget*> MyWidgets;
			//	TSubclassOf<UUserWidget> MyWidgetClass = UResultFight_Widget::StaticClass();
			//	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), MyWidgets, MyWidgetClass, false);
			//	if (MyWidgets.Num() > 0)
			//	{
			//		UResultFight_Widget* ResultWidget = Cast<UResultFight_Widget>(MyWidgets[0]);
			//		if (ResultWidget)
			//			ResultWidget->Set_CallResultWidget(true);
			//	}
			//	bStart = false;
			//	bFailed = true;
			//	R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)] = false;
			//}
		}
		else
		{
			if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
			{
				if (MyCameraManager->Get_CameraActor())
					Cam_InitPos = MyCameraManager->Get_CameraActor()->GetComponentLocation();
			}
		}
	}

	else if (true == bFailed)
	{
		// 카메라가 돌 시간
		Failed_ChangeCam_Timer += GetWorld()->GetDeltaSeconds();
		if (Failed_ChangeCam_Timer >= 1.f)
		{
			Return();
			if (false == bCamOriginPos)
			{
				if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
				{
					MyCameraManager->Get_CameraActor()->SetWorldRotation(R1Player->GetActorRotation());

					FVector CamPos = MyCameraManager->Get_CameraActor()->GetComponentLocation();
					FVector NewPos = FMath::VInterpTo(CamPos, Cam_InitPos, GetWorld()->GetDeltaSeconds(), 2.f);

					MyCameraManager->Get_CameraActor()->SetWorldLocation(NewPos);

					FVector PosDist = (R1Player->GetActorLocation() - CamPos);
					PosDist.Normalize();
					MyCameraManager->Get_CameraActor()->SetWorldRotation(PosDist.Rotation());


					if (50.f > FVector::Dist(NewPos, Cam_InitPos))
						bCamOriginPos = true;
				}
			}
			else if (true == bCamOriginPos)
			{
				if (Exit_Widget(Dialog_Instance))
				{
					Failed_ChangeCam_Timer = 0.f;
					bFailed = false;
					bCamOriginPos = false;
				}
			}
		}
	}

	// 카메라 씬 한 번 돌아가고
	// 턴 종료
}

bool ABuilding_Sign::Setting_WidgetOption()
{
	// Find Widget
	if (Dialog_Widget)
		Dialog_Instance = Cast<UHintDialog_Widget>(CreateWidget(GetWorld(), Dialog_Widget));
	else
		return false;

	if (Dialog_Instance)
		Dialog_Instance->AddToViewport();
	else
		return false;
	return true;
}
