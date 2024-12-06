// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/R1Character.h"
#include "Item/R1ItemData.h"
#include "R1Npc.generated.h"


UCLASS()
class LPROJECT_API AR1Npc : public AR1Character
{
	GENERATED_BODY()

public:
	struct Load_NpcInfo
	{
		ENPC_TYPE NpcType;
		bool bPrevLookAtRotation; // 이전에 접촉하였는가?
		bool bEndDialog;	// 대화가 끝났는가?
		int32 NextCnt;		// 현재 몇 번째 대화까지 읽었는가? 
	};


public:
	AR1Npc();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	void Pattern_Function();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	bool Exit_Widget(class UR1UserWidget* _widget);

public : // For. Getter Setter 
	void						Set_LookatPlayer(bool _look)	{ bLookAtRotation = _look; } // NPC와 상호작용하였는가?
	ENPC_TYPE				Get_NpcType()						{ return Npc_Type; } // 어떤 NPC인가?
	
	// NPC와의 대화가 끝났는가?
	bool						Get_NpcEndDialog()				{ return bEndDialog; } 
	void						Set_NpcEndDialog(bool _end)	{ bEndDialog = _end;  } 

	// NPC와 접촉 중이었나?
	bool						Get_PrevLookAtRotation()		{ return bPrevLookAtRotation;  }

	// 몇 번째 까지 읽었는가?
	int32						Get_DialogCnt()				{ return DialogNextCnt;  }
	void						Set_DialogCnt(int32 _cnt)	{ DialogNextCnt = _cnt; }

private : // For. Setting
	bool			Find_Player();
	bool			Find_Widget();
	bool			Load_Npc();


protected : // For. Widget 
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Store_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UStore_Widget> Store_Instance;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Type")
	ENPC_TYPE									Npc_Type = { ENPC_TYPE::END_MERCHANT };

	TObjectPtr<class AR1Player>			R1Player = { nullptr };
	TObjectPtr<class AMyPlayerState>	CurrentPlayerState = { nullptr };

	TArray<FItemData_Asset*>				ItemAssets;

	bool bLookAtRotation					= { false };

	int32 DialogNextCnt						= { 0 };
	bool bPrevLookAtRotation				= { false };
	bool bEndDialog							= { false };

	bool bLoadNpc							= { false };
};
