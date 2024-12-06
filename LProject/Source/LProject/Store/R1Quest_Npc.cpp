

#include "Store/R1Quest_Npc.h"
#include "Player/R1Player.h"
#include "UI/InGame/Mission_Widget.h"
#include "UI/InGame/DialogEvent_Widget.h"

#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"
#include "System/R1GameInstance.h"

#include "Components/WidgetComponent.h"
#include "Components/Image.h"
#include "UI/R1UserWidget.h"

AR1Quest_Npc::AR1Quest_Npc()
	: Super()
{
	// 3D Widget
	CurrentState_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NPC_3DWIDGET"));
	CurrentState_Widget->SetupAttachment(GetMesh());
	CurrentState_Widget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	CurrentState_Widget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> CurrentWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/InGame_Default_UI/WBP_QuestNpc.WBP_QuestNpc_C'"));
	if (CurrentWidget.Succeeded())
	{
		CurrentState_Widget->SetWidgetClass(CurrentWidget.Class);
		CurrentState_Widget->SetDrawSize(FVector2D(1000, 1000));
	}

	// Target Widget
	static ConstructorHelpers::FObjectFinder<UTexture2D> TImageFinder(TEXT("/Script/Engine.Texture2D'/Game/Art/ClassicPixelUI/UIElements/Quest.Quest'"));
	if (TImageFinder.Succeeded())
	{
		Target_QuestImage = TImageFinder.Object;
	}

	// Origin Widget
	static ConstructorHelpers::FObjectFinder<UTexture2D> OImageFinder(TEXT("/Script/Engine.Texture2D'/Game/Art/ClassicPixelUI/UIElements/Quest_off.Quest_off'"));
	if (OImageFinder.Succeeded())
	{
		Origin_QuestImage = OImageFinder.Object;
	}
}

void AR1Quest_Npc::BeginPlay()
{
	Super::BeginPlay();

	if (!Find_Widget())
		MSG_BOX(TEXT("AR1VillageForeman() : Widget이 생성되지 않았습니다."));
}

void AR1Quest_Npc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. Load Player 시
	if (true == bLoadNpc)
		Load_Npc();

	// 2. Quest 진행을 해야 하는 Npc를 가리킴
	Change_QuestImage();

	// 3. 뗀 후에 계속 Widget이 시작하는 것을 막기 위해
	End_ContextTiming(); 

	// 4. 뗀 순간
	OnDialogueEnd();
}


void AR1Quest_Npc::OnDialogueEnd()
{
	if (true == Village_Instance->Get_Exit())
	{
		if (R1Player->End_Turn(true))
		{
			Village_Instance->Set_Exit(false);
			bSpeak = false;
		}
	}

	if (bPrevLookAtRotation != bLookAtRotation)
		bSpeak = true;
}

void AR1Quest_Npc::End_ContextTiming()
{
	if (false == bTarget_QuestNpc)
		return;

	if (true == bSpeak)
	{
		if (true == bLookAtRotation)
		{
			if (false == Village_Instance->Get_Exit())
			{
				if (Village_Instance)
					Village_Instance->Set_ShowWidget(true);
			}
			bPrevLookAtRotation = bLookAtRotation;
		}
		else
			Village_Instance->Set_ShowWidget(false);
	}
}

void AR1Quest_Npc::Change_QuestImage()
{
	if (false == bChange_Image)
	{
		if (true == bTarget_QuestNpc)
		{
			if (UImage* QuestImage = Cast<UImage>(WBPCurrent_Instance->GetWidgetFromName(TEXT("Quest_Image"))))
			{
				UR1UserWidget* ChangeInstance = Cast<UR1UserWidget>(WBPCurrent_Instance);
				if (Target_QuestImage)
				{
					ChangeInstance->ChangeImage(QuestImage, Target_QuestImage);
					bChange_Image = true;
				}
			}
		}
		else
		{
			if (UImage* QuestImage = Cast<UImage>(WBPCurrent_Instance->GetWidgetFromName(TEXT("Quest_Image"))))
			{
				UR1UserWidget* ChangeInstance = Cast<UR1UserWidget>(WBPCurrent_Instance);

				if (ChangeInstance && Origin_QuestImage)
				{
					ChangeInstance->ChangeImage(QuestImage, Origin_QuestImage);
					bChange_Image = true;
				}
			}
		}
	}

	if (bPrevTarget_QuestNpc != bTarget_QuestNpc)
	{
		bChange_Image = false;
		bPrevTarget_QuestNpc = bTarget_QuestNpc;
	}
}

void AR1Quest_Npc::Load_Npc()
{
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	for (auto iter : GameInstance->Load_Npcs)
	{
		if (iter.NpcType == this->Npc_Type)
		{
			bEndDialog = iter.bEndDialog;
			bPrevLookAtRotation = bLookAtRotation = iter.bPrevLookAtRotation;
			DialogNextCnt = iter.NextCnt;

			if (Village_Instance)
			{
				Village_Instance->Set_CurrentCnt(DialogNextCnt);
				Village_Instance->Set_EndDialog(bEndDialog);
			}
		}

		bLoadNpc = false;
		break;
	}
}

bool AR1Quest_Npc::Find_Widget()
{
	/* 0. Mission Widget*/
	TArray<UUserWidget*> FoundWidgets;
	UObject* WorldContextObject = GetWorld();
	FoundWidgets = AR1Player::FindWidgetsByClass<UMission_Widget>(WorldContextObject);

	if (FoundWidgets.Num() > 0)
	{
		UMission_Widget* FoundWidget = Cast<UMission_Widget>(FoundWidgets[0]);
		if (FoundWidget)
			MissionWidget = FoundWidget;
	}

	/* 1. Dialog Widget*/
	if (Village_Widget)
		Village_Instance = Cast<UDialogEvent_Widget>(CreateWidget(GetWorld(), Village_Widget));

	if (Village_Instance)
	{
		Village_Instance->Set_NpcType(this , MissionWidget);
		Village_Instance->AddToViewport(2);
		Village_Instance->Set_ShowWidget(false);
	}
	else
		return false;

	// 2. 3D Widget Load 
	if (CurrentState_Widget)
	{
		CurrentState_Widget->InitWidget();
		WBPCurrent_Instance = CurrentState_Widget->GetUserWidgetObject();
		if (WBPCurrent_Instance)
			WBPCurrent_Instance->SetVisibility(ESlateVisibility::Visible);
		else
			return false;
	}
	else
		return false;

	bSpeak = true;
	return true;
}