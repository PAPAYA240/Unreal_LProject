// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "HerblistAbility_Widget.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHURB_TYPE : uint8
{
	DONE_HRUB, // 잡초
	DEFAULT_HURB, // 기본 허브
	GOOD_HERB, // 좋은 허브
	SUPER_HERB, // 만능 허브
	END_HERB
};

UCLASS()
class LPROJECT_API UHerblistAbility_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UHerblistAbility_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
private: //============= For. 델리게이트  =============
	UFUNCTION(BlueprintCallable)
	void OnFindButtonClicked();



public: //============= For. Getter Setter  =============
	void			Set_OwnerPlayer(class AR1Player_Herbalist* _player) { OwnerPlayer = _player;  }



private: //============= For. 함수  =============
	bool		Setting_WidgetOption();

	void		Find_Hurb(); // 허브 찾기
	void		Kind_Hurb(); // 허브 종류


protected: //============= For. Widget 함수 =============
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Docum_Text;

private: //============= For. 변수 =============
	UPROPERTY()
	class AR1Player_Herbalist*	OwnerPlayer = { nullptr };
	UPROPERTY()
	bool		bFind = { false };
	UPROPERTY()
	bool		bEnd	= { false };
	EHURB_TYPE		HurbType = { EHURB_TYPE::END_HERB };
	UPROPERTY()
	float		FindHurb_Timer = { 0 };

};
