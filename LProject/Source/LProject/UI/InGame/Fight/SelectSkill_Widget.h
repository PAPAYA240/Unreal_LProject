
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "SelectSkill_Widget.generated.h"

// 플레이어가 선택한 스킬 타입
enum class SKILL_TYPE
{
	INVENTORY,
	ATTACK,
	HIDDEN_ATTACK,
	RETREAT,
	END
};

UCLASS()
class LPROJECT_API USelectSkill_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	USelectSkill_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);


public : //============= For. Getter Setter =============
	void Use_Energy(bool _using);


private ://============= For. 함수 =============
	bool IsGameStart();
	void TextSetting(FText _accuracy, FText _damage, FText _typeName);
	void Authority_Target();

	void Skill_Pattern(float DeltaTime);
	void Change_Widget();

	bool	UpdateEnergyImage(float DeltaTime);
	void	Init_EnergyImage();


private :  //============= For. 델리게이트 =============
	UFUNCTION(BlueprintCallable)
	void OnInventoryButton_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnAttackButton_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnHiddenAttackButton_Clicked();

	UFUNCTION(BlueprintCallable)
	void OnRetreatButton_Clicked();


public: //============= For. 블루프린트 변수 =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* ChangeTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* InitTexture;


protected ://============= For. Widget 변수 =============
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Accuracy_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Damage_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> TypeName_Text;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> HiddenAttack_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Inventory_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Attack_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Retreat_Button;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Energy_Canvas;
	


private ://============= For. 변수 =============
	// 에너지 관련
	UPROPERTY()
	int32 ResultCnt = { 0 };

	UPROPERTY()
	float SaveTimer = { 0.f };

	UPROPERTY()
	TMap<class UImage*, bool> Energy_Map;

	UPROPERTY()
	bool			bButtonClick = { false }; // 버튼을 클릭했는 지 확인하여 Visible 조정

	bool bBehavior[static_cast<int32>(SKILL_TYPE::END)] = {};
};
