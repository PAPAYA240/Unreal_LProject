

#include "UI/InGame/FightState_Widget.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Monster/R1Monster.h"
#include "Player/R1Character.h"

#include "Components/TextBlock.h"

UFightState_Widget::UFightState_Widget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFightState_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Find_Owner())
		MSG_BOX(TEXT("UFightState_Widget() : Widget의 Owner를 찾을 수 없습니다."));

	if (AR1Player* OwnerPlayer = Cast<AR1Player>(R1Owner))
		bPlayer = true;
	else
		bPlayer = false;

	Set_ShowWidget(false);
}

void UFightState_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (true == bOperate)
	{
		if (true == Operate_Text())
		{
			if (true == bPlayer)
			{
				if (AR1Player* OwnerPlayer = Cast<AR1Player>(R1Owner))
				{
					if (false == OwnerPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(CurrentPlayer)])
						bOperate = false;
				}
			
			}
			else if (false == bPlayer)
			{
				if (AR1Monster* OwnerMonster = Cast<AR1Monster>(R1Owner))
				{
					if (false == OwnerMonster->Get_Monster_Pattern()[static_cast<int32>(CurrentMonster)])
						bOperate = false;
				}
			}
		}
	}
	else
	{
		// 행동 패턴 체크 : Player
		if (true == bPlayer)
			Check_PlayerPattern();
		
		// 행동 패턴 체크 : Monster
		else if (false == bPlayer)
			Check_MonsterPattern();
	}
}

void UFightState_Widget::Check_PlayerPattern()
{
	if(AR1Player* OwnerPlayer = Cast<AR1Player>(R1Owner))
	{
		// 플레이어 공격
		if (OwnerPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("공격")));
			CurrentPlayer = EPLAYER_PATTERN::ATTACK;
			bOperate = true;
		}
		else if (OwnerPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("히든 공격")));
			CurrentPlayer = EPLAYER_PATTERN::HIDDEN_ATTACK;
			bOperate = true;
		}
		else if (OwnerPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("치명타")));
			CurrentPlayer = EPLAYER_PATTERN::HIT;
			bOperate = true;
		}
		else if (OwnerPlayer->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("회피")));
			CurrentPlayer = EPLAYER_PATTERN::SHIRK;
			bOperate = true;
		}
		else if (OwnerPlayer->Get_CurrentPlayerState()->TargetMonster)
		{
			if (true == OwnerPlayer->Get_CurrentPlayerState()->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
			{
				CurrentState_Text->SetText(FText::FromString(TEXT("승리함")));
				bOperate = true;
			}
		}
	}

	if (true == bOperate)
		Set_ShowWidget(true);
	else
		Set_ShowWidget(false);
}

void UFightState_Widget::Check_MonsterPattern()
{
	if (AR1Monster* OwnerMonster = Cast<AR1Monster>(R1Owner))
	{
		// 플레이어 공격
		if (true == OwnerMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("공격")));
			CurrentMonster = EMONSTER_PATTERN::ATTACK;
			bOperate = true;
		}
		else if (true == OwnerMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("히든 공격")));
			CurrentMonster = EMONSTER_PATTERN::HIDDEN_ATTACK;
			bOperate = true;
		}
		else if (true == OwnerMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIT)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("치명타")));
			CurrentMonster = EMONSTER_PATTERN::HIT;
			bOperate = true;
		}
		else if (true == OwnerMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
		{
			CurrentState_Text->SetText(FText::FromString(TEXT("처치")));
			CurrentMonster = EMONSTER_PATTERN::DEAD;
			bOperate = true;
		}
	}

	if (true == bOperate)
		Set_ShowWidget(true);
	else
		Set_ShowWidget(false);
}

bool UFightState_Widget::Operate_Text()
{
	FVector2D TargetPos = OriginPos;
	TargetPos.Y -= 200.f;

	FVector2D NewPos = VInterpTo2D(GetTextPosition(CurrentState_Text), TargetPos, GetWorld()->GetDeltaSeconds(), 3.f);
	MoveText(CurrentState_Text, NewPos);

	if (10.f >= FVector2D::Distance(NewPos, GetTextPosition(CurrentState_Text)))
	{
		Text_Timer += GetWorld()->GetDeltaSeconds();
		if (1.f <= Text_Timer)
		{
			Text_Timer = 0.f;
			MoveText(CurrentState_Text, OriginPos);
			return true;
		}
	}
	return false;
}

bool UFightState_Widget::Find_Owner()
{
	OriginPos = GetTextPosition(CurrentState_Text);
	APawn* OwningPawn = GetOwningPlayerPawn();
	R1Owner = Cast<AR1Character>(OwningPawn);
	if (!R1Owner)
		return false;
	else
		return true;
}
