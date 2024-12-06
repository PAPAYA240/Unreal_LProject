// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "MonsterEnergy_Widget.generated.h"

/* 몬스터 공격 판별 */
UCLASS()
class LPROJECT_API UMonsterEnergy_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UMonsterEnergy_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public : //============= For. Getter Setter =============
	bool					Get_MonsterAttack_Start() 
	{ 
		ShowWidget_Timer = 0.f;
		return bAttackStart; 
	}

	void					Set_Monster(class AR1Monster* _monster) { R1Monster = _monster; };
	void					Set_EnergyStart(bool _start) { bStartEnergy = _start; }

protected:
	virtual void			NativeConstruct() override;
	virtual void			NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private: //============= For. 함수 =============
	bool					Show_Widget();
	bool					UpdateEnergyImage(float DeltaTime);
	void					Init_EnergyImage();



public:  //============= For. Widget 변수 =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* ChangeTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* InitTexture;

protected :
	// Player 당 에너지 들어가 있는 Map
	TMap<class UImage*, bool>		Energy_Map;
	TObjectPtr<class AR1Monster>	R1Monster;


private : //============= For. 변수 =============
	UPROPERTY()
	float SaveTimer = { 0.f };
	UPROPERTY()
	bool bStartEnergy = { false };
	UPROPERTY()
	bool bAttackStart = { false };
	UPROPERTY()
	int32 ResultCnt = { 0 };
	// 에너지를 전부 채웠다면 0.2초 후에 끊어줄 예정
	UPROPERTY()
	float ShowWidget_Timer = { 0.f }; 
	// 
	UPROPERTY()
	bool bFullEnergy = { false };
};
