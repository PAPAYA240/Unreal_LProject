// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "DialogEvent_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UDialogEvent_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UDialogEvent_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 빌딩 타입을 넘김
	void Set_BuildingType(EBUILDING_TYPE _build) { Building_Type = _build; }
	// NPC 타입을 넘김
	void Set_NpcType(class AR1Npc* _npc, class UMission_Widget* _mission)
	{
		R1Npc = _npc;
		bNpc = true;
		MissionWidget = _mission;
	}

	int32 Get_CurrentCnt() { return NextCnt;  }
	void  Set_CurrentCnt(int32 _cnt) { NextCnt = _cnt; }

	void	Set_EndDialog(bool _end) { bDialogEnd = _end; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnContinue_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnContect_Clicked();

private :
	void Change_NpcResource();
	void Change_BuildingResource(); // 빌딩 타입에 따라 내용이 달라짐
	
	// For. NPC
	void Villageforman_Dialog();

	void Marry_Dialog();

	void MarryFather_Dialog();

	// For. Setting
	bool Setting_WidgetOption();

protected:
	// 이름
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Name_Text;

	// 이름 설명
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> SideName_Text;

	// 이름 설명
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Dialog_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Contect_Canvas;

private:
	TObjectPtr<class UMission_Widget> MissionWidget;
	TObjectPtr<class AR1Npc>				R1Npc;
	EBUILDING_TYPE							Building_Type		= EBUILDING_TYPE::END_BUILDING; 
	bool											bChangeText		= { false };

private : // NPC
	int32											NextCnt				= { 0 }; // 다음 Dialog로 넘어가는 변수
	int32											MaxDialogCnt		= { 0 };
	bool											bNpc					= { false };
	bool											bDialogEnd			= { false };
};
