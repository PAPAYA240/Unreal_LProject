
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "ReadyFight_Widget.generated.h"

/* 몬스터와의 상호작용 조건을 판단한다 */
enum class READY_TYPE
{
	ATTACK,
	AMBUSH,
	RETREAT,
	END
};

UCLASS()
class LPROJECT_API UReadyFight_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UReadyFight_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public : //============= For. Getter Setter =============
	void Set_Monster(class AR1Monster* _monster) { R1Monster = _monster;  }
	void Use_Energy(bool _using);

protected :
	bool			Setting_Widget();
	bool			Setting_Text();

private :  //============= For. 델리게이트  =============
	UFUNCTION(BlueprintCallable)
	void OnFightButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnAmbushButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnRetreatButtonClicked();

private ://============= For. 함수 =============
	void		Ambush_Probability(float DeltaTime);		// 기습
	void		Retreat_Probability(float DeltaTime);		// 후퇴
	void		Reset_Probability();

	bool		UpdateEnergyImage(float DeltaTime);
	bool		CheckEnergyStates();
	bool		Show_Widget();

	//확률창
	void Hovering_RandomAbilityWidget();
	void RamdomAbility_Change(bool _result);

protected ://============= For. Widget 함수 =============
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Fight_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Ambush_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Retreat_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Energy_1;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Energy_2;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Energy_3;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Random_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> RandomResult_Canvas;
	
protected://============= For. Sound =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* ContactMonster_Sound;

public : //============= For. 블루프린트 변수 =============
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
    UTexture2D* ChangeTexture; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* RandomFailed_Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* RandomSucess_Texture;

private://============= For. 변수 =============
	UPROPERTY( )
	TMap<class UImage*, bool> Energy_Map;

	UPROPERTY( )
	TObjectPtr<class AR1Monster> R1Monster = { nullptr };

	bool bBehavior[static_cast<int32>(READY_TYPE::END)] = {};

	UPROPERTY( )
	float SaveTimer = { 0.f };

	UPROPERTY( )
	int32 ResultCnt = { 0 };

	UPROPERTY()
	int32 RandomResult_Cnt = { 0 };

	UPROPERTY( )
	bool bSuccess_Ambush = { true };
	
	UPROPERTY()
	bool bOpenWidget = { false };

	UPROPERTY()
	bool bReadyFight = { false };
};
