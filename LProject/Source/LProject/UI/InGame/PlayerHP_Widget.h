
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "PlayerHP_Widget.generated.h"


UCLASS()
class LPROJECT_API UPlayerHP_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UPlayerHP_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public : // For. Setter Getter 
	void Set_HpProgressBar(float _percent);

	bool Use_Energy();
	void Full_Energy(bool _full) { bFull_Energy = _full; }
	bool Add_Energy(int32 _addCnt);// Add Cross
	
	void Set_Player(class AR1Player* _player) { R1Player = _player; }
	void Set_CanvasPos(int32 _cnt);

	// MaxHp / Hp
	void Set_HpText(float _maxHp, float _hp);
	void Set_AttackText(float _attack);
	void Set_HillText(float _hill);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private :
	void Setting_NickName();
	void PlayerStart_Setting();
	void Adjustment_Opacity(bool _opacity); // Color 변경

	void TranslateScaled();
	void ChangeScaled(); // 크기 변환 시
	void ReturnScaled(); // 크기 원상복구

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel>	Cross_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UProgressBar>	RedHPBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UProgressBar>	BlueHPBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		PlayerName_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		Hp_Text;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		Attack_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>	Hill_Text;

private:
	TArray<class UImage*> EnergyArray;

	FVector2D		Canvas_InitPos;
	FVector2D		NonActive_Pos;
	FVector2D		Canvas_Scaled;

	bool bFull_Energy		= { false };
	bool bMyTurn			= { false };

	// 크기 변환
	bool bScaleTranslate = { false }; 
	float ScaledTimer		= { 0.f };
};
