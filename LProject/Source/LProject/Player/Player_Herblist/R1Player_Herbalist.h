// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/R1Player.h"
#include "R1Player_Herbalist.generated.h"


UCLASS()
class LPROJECT_API AR1Player_Herbalist : public AR1Player
{
	GENERATED_BODY()

public:
	AR1Player_Herbalist();

public:
	void Tick(float DeltaTime);

protected:
	void BeginPlay();
	virtual void Pattern_Function();

public : //============= For. Getter Setter  =============
	bool				Get_FindHerb() { return bFindHerb; }
	void				Set_FindHerb(bool _find) { bFindHerb = _find;  }

protected :
	bool		Setting_PlayerMaterial();

private:
	bool		Setting_Widget();
	bool		Setting_Weapon();

private : //============= For. Widget 변수 =============
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget>	HerblistAility_Widget;
	UPROPERTY()
	class UHerblistAbility_Widget*			HerblistAility_Instance = { nullptr };

private : //============= For. 변수 =============
	UPROPERTY()
	bool			bFindHerb = { false };
	UPROPERTY() // Hidden Attack이 끝나고 2.f 뒤에 다음 턴으로 넘길 때 사용
	float			HiddenAttackEnd_Timer = { 0.f };
};
