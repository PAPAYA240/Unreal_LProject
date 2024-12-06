

#include "UI/Lobby/ClassInfo_Widget.h"
#include "Player/Player_Lumberjack/R1Player_Lumberjack.h"
#include "Player/R1PlayerController.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Character.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"

#include "UI/Lobby/PlayerSetting_Widget.h"
#include "UI/Lobby/AbilityInfo_Widget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

#include "Camera/CameraManager.h"

#include "R1GameModeBase.h"
#include "Kismet/GameplayStatics.h"

UClassInfo_Widget::UClassInfo_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UClassInfo_Widget::Get_JobName()
{
	return JobName->GetText();
}

void UClassInfo_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if(!Setting_Widget())
		MSG_BOX(TEXT("UClassInfo_Widget() : Widget Setting이 되지 않았습니다."));
}

void UClassInfo_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 카메라 찾기
	if(false == bSettingPos)
	{
		ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
		if (MyCameraManager)
		{
			if (MyCameraManager->Get_CameraActor())
			{
				CameraComponent = MyCameraManager->Get_CameraActor();
				if(CameraComponent)
				{
					SelectPlayer_LocationCam = Init_LocationCam = CameraComponent->GetComponentLocation();
					bSettingPos = true;
				}
			}
		}
	}
}

void UClassInfo_Widget::CheckHoveringImages()
{
	FVector2D MousePosition;
	if (GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		if(AbilityCanvas)
		{
			for (UWidget* Child : AbilityCanvas->GetAllChildren())
			{
				UImage* Image = Cast<UImage>(Child);
				if (Image)
				{
					UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image->Slot);
					if (CanvasSlot)
					{
						FVector2D ImagePosition = CanvasSlot->GetPosition();
						FVector2D ImageSize = Image->GetDesiredSize();

						// 호버링 감지
						if (MousePosition.X >= ImagePosition.X && MousePosition.X <= ImagePosition.X + ImageSize.X &&
							MousePosition.Y >= ImagePosition.Y && MousePosition.Y <= ImagePosition.Y + ImageSize.Y)
						{
							MSG_BOX(TEXT("호버링 처리"))

							//  처리
							HoveringInfo.HoveringPos = ImagePosition;
							HoveringInfo.bHovering = true;

							FString ImageName = Image->GetName();
							if (ImageName.Equals(TEXT("Power")))
								HoveringInfo.abilityType = ABILITY_TYPE::POWER;

							else if (ImageName.Equals(TEXT("Defence")))
								HoveringInfo.abilityType = ABILITY_TYPE::DEFENCE;

							else if (ImageName.Equals(TEXT("Speed")))
								HoveringInfo.abilityType = ABILITY_TYPE::SPEED;

							else if (ImageName.Equals(TEXT("Intellectual")))
								HoveringInfo.abilityType = ABILITY_TYPE::INTELLECTUAL;

							else if (ImageName.Equals(TEXT("Hill")))
								HoveringInfo.abilityType = ABILITY_TYPE::HILL;

							else if(ImageName.Equals(TEXT("Lucky")))
								HoveringInfo.abilityType = ABILITY_TYPE::LUCKY;
							else
								HoveringInfo.abilityType = ABILITY_TYPE::END;

							AbilityBoxWidget_Instance->Set_AbilityBox_HoveringInfo(HoveringInfo);
							MSG_BOX(TEXT("호버링"))

							return;
						}

						// not hovering
						HoveringInfo = {};
						AbilityBoxWidget_Instance->Set_AbilityBox_HoveringInfo(HoveringInfo);
					}
				}
			}
		}
	}
}

void UClassInfo_Widget::Change_SkeletalMesh()
{
	if (!PlayerSetting_Instance->SelectPlayer)
		return;

	USkeletalMeshComponent* Mesh = PlayerSetting_Instance->SelectPlayer->FindComponentByClass<USkeletalMeshComponent>();
	if (Mesh)
	{
		if (Job_SkeletalMesh[static_cast<int32>(PlayerSetting_Instance->SelectPlayer->Get_CurrentPlayerState()->Player_Job)])
			Mesh->SetSkeletalMesh(Job_SkeletalMesh[static_cast<int32>(PlayerSetting_Instance->SelectPlayer->Get_CurrentPlayerState()->Player_Job)]);
	}
}

void UClassInfo_Widget::OnLeftArrowClicked()
{
	if (static_cast<int>(JobType) <= 0)
		JobType = static_cast<EJOB_TYPE>(static_cast<int32>(EJOB_TYPE::END) - 1);
	else
		JobType = static_cast<EJOB_TYPE>(static_cast<int>(JobType) - 1);

	Change_Job();
}

void UClassInfo_Widget::OnRightArrowClicked()
{
	JobType = static_cast<EJOB_TYPE>(static_cast<int>(JobType) + 1);

	if (EJOB_TYPE::END <= JobType)
		JobType = static_cast<EJOB_TYPE>(0);

	Change_Job();
}

void UClassInfo_Widget::OnExitClicked()
{
	if(PlayerSetting_Instance)
		PlayerSetting_Instance->Set_ShowWidget(ESlateVisibility::Visible);

	Set_ShowWidget(true);
	bSelectPlayer = false; // 다시 플레이어 고르기 가능
}

void UClassInfo_Widget::Change_Job()
{
	FText SelectJobName, SelectJobInfo;
	switch (JobType)
	{
	case EJOB_TYPE::LUMBERJACK:
		SelectJobName = FText::FromString(TEXT("나무꾼"));
		SelectJobInfo =  FText::FromString(TEXT(
			"전설적인 금속의 예술가이자 전투의 마스터입니다.\n"
			"그들은 강력한 무기와 방어구를 제작하는 능력으로 유명하며,\n"
			"활력, 힘, 재능이 뛰어난 클래스입니다.\n\n"
			"대장장은 전투에서 직접적으로 적과 맞서 싸우는 것뿐만 아니라,\n"
			"팀원에게 장비를 제공해 전투를 지원하는 역할도 수행합니다."));
		break;

	case EJOB_TYPE::WIZARD:
		SelectJobName = FText::FromString(TEXT("사냥꾼"));
		SelectJobInfo = FText::FromString(TEXT(
			"자연의 정글과 숲을 누비며 생존 기술을 익힌 전문가입니다.\n"
			"뛰어난 추적 능력으로 야생 동물을 사냥하고 보물을 발견합니다.\n"
			"함정을 활용해 적을 제압하고 자원을 확보합니다.\n\n"
			"은신과 기민함으로 기습 공격으로 전투에 큰 영향을 미치며.\n"
			"야생에서 강력한 능력을 발휘하는 직업입니다."));
		break;

	case EJOB_TYPE::HERBALIST:
		SelectJobName = FText::FromString(TEXT("약초상"));
		SelectJobInfo = FText::FromString(TEXT(
			"지혜를 지닌 약초 수집가이자 치유의 전문가입니다.\n"
			"맑은 낮시간에 빈 땅에 서 있을 때, 그들은 특별한 능력으로\n"
			"약초를 발견하는 뛰어난 재능을 발휘합니다.\n\n"
			"강력한 치유 물약을 제작하며, 전투에서 팀의 생존력을 높이는\n"
			"중요한 역할을 수행합니다."));
		break;

	default:
		SelectJobName = FText::FromString(TEXT("알 수 없는 직업"));
		SelectJobInfo = FText::FromString(TEXT("알 수 없는 직업"));
		break;
	}

	JobName->SetText(SelectJobName);
	Job_Info->SetText(SelectJobInfo);

	AMyPlayerState* PlayerState = R1Player->GetPlayerState<AMyPlayerState>();

	// 플레이어 메시 바꾸기
	if (nullptr != PlayerSetting_Instance->SelectPlayer)
	{
		PlayerSetting_Instance->SelectPlayer->Get_CurrentPlayerState()->Player_Job = JobType;
		Change_SkeletalMesh();
	}
}

// 준비 시
void UClassInfo_Widget::Camera_Setting(bool _setting)
{
	const float fDirection = 130.f;
	if (true == _setting)
	{
		if(true == bSelectPlayer)
		{
			if (CameraComponent)
			{
				// 카메라의 현재 위치를 가져옵니다.
				FVector vCurrentLocationCam = CameraComponent->GetComponentLocation(); // GetActorLocation() 대신 GetComponentLocation() 사용
				FVector vTargetLocation = FVector(vCurrentLocationCam.X, SelectPlayer_LocationCam.Y + fDirection, vCurrentLocationCam.Z);
				
				// 현재 위치에서 목표 위치로 부드럽게 이동
				FVector vNewLocation = FMath::VInterpTo(vCurrentLocationCam, vTargetLocation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
				CameraComponent->SetWorldLocation(vNewLocation); // SetActorLocation() 대신 SetWorldLocation() 사용
			}
		}
	}

	else if (false == _setting)
	{
		if (CameraComponent)
		{
			FVector vCurrentLocationCam = CameraComponent->GetComponentLocation();
			FVector vTargetLocation = FVector(vCurrentLocationCam.X, Init_LocationCam.Y, vCurrentLocationCam.Z);

			FVector vNewLocation = FMath::VInterpTo(vCurrentLocationCam, vTargetLocation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
			CameraComponent->SetWorldLocation(vNewLocation);
		}
	}
}

AR1Player* UClassInfo_Widget::InitPos_Setting()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		for (int i = 0; i < LAST_PLAYER; i++)
		{
			AR1PlayerController* Controller = Cast<AR1PlayerController>(GetWorld()->GetFirstPlayerController());
			if (Controller)
			{
				if (Controller->Highlight_Character())
				{
					if (GameMode->PlayerArray[i] == Controller->Highlight_Character())
					{
						SelectPlayer_LocationCam.Y = Controller->Highlight_Character()->GetActorLocation().Y;
						bSelectPlayer = true;

						AR1Player* SelectPlayer = Cast<AR1Player>(Controller->Highlight_Character());

						return SelectPlayer;
					}
				}
			}
		}
	}
	return nullptr;
}

bool UClassInfo_Widget::Setting_Widget()
{
	/* Ability Box Widgetr*/
	AbilityBoxWidget_Instance = Cast<UAbilityInfo_Widget>(CreateWidget(GetWorld(), AbilityBox_Widget));
	if (AbilityBoxWidget_Instance)
		AbilityBoxWidget_Instance->AddToViewport();
	else
		MSG_BOX(TEXT("Failed : PlayerHP_Instance"));

	if (UButton* LeftArrow = Cast<UButton>(GetWidgetFromName(TEXT("Left_Arrow"))))
		LeftArrow->OnClicked.AddDynamic(this, &UClassInfo_Widget::OnLeftArrowClicked);
	else
		return false;

	if (UButton* RightArrow = Cast<UButton>(GetWidgetFromName(TEXT("Right_Arrow"))))
		RightArrow->OnClicked.AddDynamic(this, &UClassInfo_Widget::OnRightArrowClicked);
	else
		return false;

	if (UButton* ButtonExit = Cast<UButton>(GetWidgetFromName(TEXT("Button_Exit"))))
		ButtonExit->OnClicked.AddDynamic(this, &UClassInfo_Widget::OnExitClicked);
	else
		return false;
	return true;
}
