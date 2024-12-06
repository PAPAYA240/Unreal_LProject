// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/R1Character.h"
#include "Define/R1Define.h"
#include "R1LoadPlayer.generated.h"


UCLASS()
class LPROJECT_API AR1LoadPlayer : public AR1Character
{
	GENERATED_BODY()
	
public :
	struct Load_PlayerInform
	{
		// Load Player
		EJOB_TYPE						_JobType; 
		ESCENE							Player_CurrentScene = { ESCENE::LOBBY };
		ESCENE							Player_PrevScene = { ESCENE::LOBBY };

		TArray<EMONSTER_TYPE> Target_MonsterType;
		int32								Monster_Number;
		FVector							LastPlayerPosition;

		bool								bTarget_Player = { false }; // 마지막에 컨트롤을 잡고 있던 플레이어 구분
		EMISSION_TYPE				MissionType = { EMISSION_TYPE::VISIT_VILLAGEMAN };
		int32								Current_DeadLine;

		int32								Total_Coin;
		int32								CurrentLevel;
		float								Hp;
		int32								MaxHp;
		int32								CurrentAttack;
		int32								Hill;

		class UMaterialInterface* Registration_Material[static_cast<int32>(CUSTOMIZE_PLAYER::END_CUSTOM)];

		bool								bDead = { false };
	};

	AR1LoadPlayer();

public : // For. Getter Setter
	class AMyPlayerState*	Get_CurrentPlayerState() { return CurrentPlayerState; }
	void							Set_CurrentPlayerState(class AMyPlayerState* _state) { CurrentPlayerState = _state;  }

	bool							Get_LoadPlayer() { return bLoadPlayer;  }
	void							Set_LoadPlayer(bool _load) { bLoadPlayer = _load; }

protected :
	virtual void BeginPlay() override;

public :
	bool			Save_Player();
	bool			Setting_Material();


protected :	
	bool			Load_Completed();


protected :
	TObjectPtr<class AMyPlayerState>				CurrentPlayerState = { nullptr };
	// 로드된 플레이어인가? ; Lobby에서 처음 시작될 땐 예외함
	UPROPERTY()
	bool bLoadPlayer = { false };
	// 개별적으로 로드해서 변경해야 하는 부분
	UPROPERTY()
	bool	bIndependentLoading = { false };
};
