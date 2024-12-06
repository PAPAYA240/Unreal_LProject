#include "UI/InGame/DialogEvent_Widget.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

#include "Interactive_Buildings/Interactive_Buildings.h"
#include "UI/InGame/Mission_Widget.h"
#include "Store/R1Quest_Npc.h"
#include "Store/R1Npc.h"

UDialogEvent_Widget::UDialogEvent_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDialogEvent_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if(!Setting_WidgetOption())
		MSG_BOX(TEXT("UDialogEvent_Widget() : Widget Option이 제대로 세팅되지 않았습니다."))

	TArray<UUserWidget*> FoundWidgets;
	UObject* WorldContextObject = GetWorld();
	FoundWidgets = AR1Player::FindWidgetsByClass<UMission_Widget>(WorldContextObject);

	if (FoundWidgets.Num() > 0)
	{
		UMission_Widget* FoundWidget = Cast<UMission_Widget>(FoundWidgets[0]);
		if (FoundWidget)
			MissionWidget = FoundWidget;
	}
}

void UDialogEvent_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Building Dialog
	if (false == bNpc) 
	{
		Change_BuildingResource();
	}
	// NPC Dialog
	else if (true == bNpc)
	{
		
		Change_NpcResource();
	}
}

void UDialogEvent_Widget::OnContinue_Clicked()
{
	// 1. Building Dialog
	if(false == bNpc)
	{
		Set_ShowWidget(false);
		bExit = true;
	}
	// 2. NPC Dialog
	else if (true == bNpc)
	{
		bChangeText = false;
		++NextCnt;

		// 마지막 대화 시
		if (NextCnt >= MaxDialogCnt)
		{
			SideName_Text->SetText(FText::FromString(TEXT("종료하기")));
		}

		// 창 끄기
		if (NextCnt > MaxDialogCnt)
		{
			if (EMISSION_TYPE::VISIT_VILLAGEMAN == MissionWidget->Get_CurrentMission())
			{
				if (MissionWidget)
					MissionWidget->Setting_MissionType(EMISSION_TYPE::VISIT_VILLAGEMAN_KILLMONSTER_MISSION);
			}

			Set_ShowWidget(false);
			for (auto iter : Canvas->GetAllChildren())
				iter->SetVisibility(ESlateVisibility::Hidden);

			if (UCanvasPanel* ContectCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Contect_Canvas"))))
				ContectCanvas->SetVisibility(ESlateVisibility::Visible);

			R1Npc->Set_NpcEndDialog(true);
			R1Npc->Set_DialogCnt(NextCnt);
			bExit = true;
		}
	}
}

void UDialogEvent_Widget::OnContect_Clicked()
{
	R1Player->End_Turn(false);

	for (auto iter : Canvas->GetAllChildren())
		iter->SetVisibility(ESlateVisibility::Visible);

	if (UCanvasPanel* ContectCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Contect_Canvas"))))
		ContectCanvas->SetVisibility(ESlateVisibility::Hidden);
}

// NPC Dialog 
void UDialogEvent_Widget::Change_NpcResource()
{
	if (nullptr == R1Npc)
		return;

	if (false == bChangeText)
	{
		switch (R1Npc->Get_NpcType())
		{
		case ENPC_TYPE::VILLAGEFORMAN_NPC :
		{
			if (nullptr == MissionWidget) 
				return;
			Villageforman_Dialog();
		}
		break;
		case ENPC_TYPE::MARRY_NPC:
		{
			if (nullptr == MissionWidget) 
				return;
			Marry_Dialog();
		}
		break;

		case ENPC_TYPE::MARRY_FATHER:
		{
			if (nullptr == MissionWidget)
				return;
			MarryFather_Dialog();
		}
		break;
		default:
			break;
		}
		bChangeText = true;
	}
}

// Building Dialog
void UDialogEvent_Widget::Change_BuildingResource()
{
	if (false == bChangeText)
	{
		switch (Building_Type)
		{
		case EBUILDING_TYPE::PLAYER_START_SPAWNER_BUILIDING:
			break;

		case EBUILDING_TYPE::HILL_BUILDING:
			Name_Text->SetText(FText::FromString(TEXT("회복의 정령")));
			Dialog_Text->SetText(FText::FromString(TEXT("언젠가 당신은 모든 정령의 축복을 받을거예요")));
			break;

		case EBUILDING_TYPE::END_BUILDING:
			break;

		default:
			break;
		}
		bChangeText = true;
	}
}

// 셀레시아 [마을 이장]
void UDialogEvent_Widget::Villageforman_Dialog()
{
	Name_Text->SetText(FText::FromString(TEXT("마을 이장")));

	if (EMISSION_TYPE::VISIT_VILLAGEMAN == MissionWidget->Get_CurrentMission())
	{
		MaxDialogCnt = 6;
		switch (NextCnt)
		{
		case 0:
			Dialog_Text->SetText(FText::FromString(TEXT("허허, 그대들이 카오스를 없애줄 용사인가? 아주 용맹스럽군.")));
			break;

		case 1:
			Dialog_Text->SetText(FText::FromString(TEXT("카오스는 세상에 혼돈을 불러일으키는 아주 사악한 몬스터야 \n자네가 오기까지 우리는 두려움에 떨 수 밖에 없었지.")));
			break;

		case 2:
			Dialog_Text->SetText(FText::FromString(TEXT("보이는가? 이게 바로 카오스가 얼마나 다가왔는 지 알 수 있는 지표라네.")));
			break;

		case 3:
			Dialog_Text->SetText(FText::FromString(TEXT("응? 카오스를 어떻게 해치울 수 있냐고?\n 허허, 아주 용맹하군, 내가 아주 사람을 잘 보았어!")));
			break;

		case 4:
			Dialog_Text->SetText(FText::FromString(TEXT("간단하네, 들판에 깔려있는 카오스의 수하를 하나씩 처치하면 되네.")));
			break;

		case 5:
			Dialog_Text->SetText(FText::FromString(TEXT("단, 이 수하들마다 실력은 천차 만별이니, 주의하며 덤비게나. \n자칫 잘못하면 그들이 목숨을 가져가는 건 한 순간이니..!")));
			break;

		case 6:
			Dialog_Text->SetText(FText::FromString(TEXT("부디 행운을 빌게.")));
			break;

		default:
			break;
		}
	}

	else if (EMISSION_TYPE::VISIT_VILLAGEMAN_KILLMONSTER_MISSION == MissionWidget->Get_CurrentMission() &&
		true == MissionWidget->Get_MissionClear())
	{
		MaxDialogCnt = 2;
		switch (NextCnt)
		{
		case 0:
			Dialog_Text->SetText(FText::FromString(TEXT("고맙네, 난 자네가 해낼 것을 알고 있었어.")));
			break;

		case 1:
			Dialog_Text->SetText(FText::FromString(TEXT("지칠 때는 마을의 명소에 들려주시게나, 치유하는데 도움이 되거든.")));
			break;

		case 2:
			Dialog_Text->SetText(FText::FromString(TEXT("참, 그러고 보니 메리가 자넬 찾고 있어. 한 번 가보게나.")));
			bDialogEnd = true;
			MissionWidget->Setting_MissionType(EMISSION_TYPE::VISIT_MARRY);
			break;

		default:
			Dialog_Text->SetText(FText::FromString(TEXT("그대가 카오스를 물리칠 영웅이길 바라네...")));
			break;
		}
	}
}

// 셀레시아 상점 딸 [메리]
void UDialogEvent_Widget::Marry_Dialog()
{
	Name_Text->SetText(FText::FromString(TEXT("메리")));

	//if (EMISSION_TYPE::VISIT_MARRY == MissionWidget->Get_CurrentMission())
	if (true == Cast<AR1Quest_Npc>(R1Npc)->Get_TargetQuestNpc())
	{
		MaxDialogCnt = 7;
		switch (NextCnt)
		{
		case 0:
			Dialog_Text->SetText(FText::FromString(TEXT("왜 이렇게 늦는 거야? 빨리 빨리 다니라고\n기다리고 있었잖아!")));
			break;

		case 1:
			Dialog_Text->SetText(FText::FromString(TEXT("너는 카오스를 어떻게 처치하는 지 알아?")));
			break;

		case 2:
			Dialog_Text->SetText(FText::FromString(TEXT("뭐야? 그런 것도 모르고 영웅이라니, 정말 한심하잖아? \n이 메리님이 한 수 가르쳐 주도록 하지!")));
			break;

		case 3:
			Dialog_Text->SetText(FText::FromString(TEXT("자 보여? 이게 바로 카오스의 정찰탑이야.\n이걸 부수면 더이상 카오스가 다가오는 게 상대적으로 느려지지.")));
			break;

		case 4:
			Dialog_Text->SetText(FText::FromString(TEXT("동시에 정찰탑을 부수게 되면 저어기 하늘에 떠 있는 북쪽 카오스의 탑을 조금씩 내릴 수 있어.\n간단하지?")));
			break;

		case 5:
			Dialog_Text->SetText(FText::FromString(TEXT("단, 이 정찰탑은 담당 지역의 카오스의 수하들을 일정 이상 처치해야 부술 수 있어, 이 녀석들이 정찰탑을 감시하고 있거든\n각 지역마다 정찰탑이 있으니까 잘 부숴보라고.")));
			break;

		case 6:
			Dialog_Text->SetText(FText::FromString(TEXT("물론 이 지역은 이 유능하신 메리님이 알아봤지. 정찰탑의 위치를 알려줄게\n두 번은 안알려주니 확실하게 기억해둬!")));
			break;
		case 7:
		{
			Dialog_Text->SetText(FText::FromString(TEXT("이 구역의 정찰탑을 부수고, 우리 아빠에게로 가.\n분명 정찰탑 근방에서 상점을 하고 있을테니 금방 찾을테지.")));
			if (MissionWidget)
				MissionWidget->Set_CurrentMission(EMISSION_TYPE::SELISIA_TOWER_BREAK_MISSION);
		}
		break;
			
		default:
			Dialog_Text->SetText(FText::FromString(TEXT("말 걸지마, 바보야!")));
			break;
		}
	}
}

// 셀레시아 상점 딸 [메리의 아빠]
void UDialogEvent_Widget::MarryFather_Dialog()
{
	Name_Text->SetText(FText::FromString(TEXT("메모리")));

	if (true == Cast<AR1Quest_Npc>(R1Npc)->Get_TargetQuestNpc())
	{
		MaxDialogCnt = 5;
		switch (NextCnt)
		{
		case 0:
			Dialog_Text->SetText(FText::FromString(TEXT("딸아이 통신으로 전해들었네,\n 자네들이 셀레시아에서 온 영웅이라지?")));
			break;

		case 1:
			Dialog_Text->SetText(FText::FromString(TEXT("꼴을 보아하니 여기까지 오는데 영웅치고 시간이 걸렸지만 말이야.")));
			break;

		case 2:
			Dialog_Text->SetText(FText::FromString(TEXT("그래도 여기까지 왔으니 칭찬은 해주겠네만, 이제부터 자네들이 정찰탑을 찾아야 하네.")));
			break;

		case 3:
			Dialog_Text->SetText(FText::FromString(TEXT("알려달라고? 난 딸아이만큼 친절하지 않아")));
			break;

		case 4:
			Dialog_Text->SetText(FText::FromString(TEXT("하지만 여기까지 온 보상으로 이걸 전해달라 하더군. \n물론 내가 줄 건 없지만 말이야.")));
			break;

		case 5:
		{
			Dialog_Text->SetText(FText::FromString(TEXT("대신으로 뭐하지만 필요한 물품이 있다면 우리 상점으로 와, 싸게 쳐주도록 하지.")));
			if (MissionWidget)
				MissionWidget->Set_CurrentMission(EMISSION_TYPE::SELISIA_TOWER_BREAK_MISSION);
		}
			break;
		break;

		default:
			Dialog_Text->SetText(FText::FromString(TEXT("말 걸지마, 바보야!")));
			break;
		}
	}
}

bool UDialogEvent_Widget::Setting_WidgetOption()
{
	// Button 등록
	if (UButton* ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("Continue_Button"))))
		ContinueButton->OnClicked.AddDynamic(this, &UDialogEvent_Widget::OnContinue_Clicked);
	else
		return false;

	Set_ShowWidget(false);

	if (true == bNpc)
	{
		Set_ShowWidget(true);
		for (auto iter : Canvas->GetAllChildren())
			iter->SetVisibility(ESlateVisibility::Hidden);

		if (UCanvasPanel* ContectCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Contect_Canvas"))))
			ContectCanvas->SetVisibility(ESlateVisibility::Visible);
		else
			return false;

		if (UButton* Contect_Button = Cast<UButton>(GetWidgetFromName(TEXT("Contect_Button"))))
			Contect_Button->OnClicked.AddDynamic(this, &UDialogEvent_Widget::OnContect_Clicked);
		else
			return false;
		
	}
	return true;
}
