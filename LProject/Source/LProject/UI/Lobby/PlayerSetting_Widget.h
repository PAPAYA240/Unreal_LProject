
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "PlayerSetting_Widget.generated.h"

UCLASS()
class LPROJECT_API UPlayerSetting_Widget : public UR1UserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerSetting_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Set_ShowWidget(ESlateVisibility _visible);

	TArray<class UMaterialInterface*> Get_HairMaterial() { return HairMaterials; }
	TArray<class UMaterialInterface*> Get_SkinMaterial() { return SkinMaterials; }
	TArray<class UMaterialInterface*> Get_TopMaterial() { return TobMaterials;  }

protected :
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private :
	bool Setting_Widget();
	void Show_ClassInfo();

	// 커스텀
	void	Change_Customize();
	void	Player_Customize(int32 _materialCnt);

private :
	UFUNCTION(BlueprintCallable)
	void OnSelectJobClicked();
	UFUNCTION(BlueprintCallable)
	void OnStartClicked();
	UFUNCTION(BlueprintCallable)
	void OnCustomizeClicked();
	UFUNCTION(BlueprintCallable)
	void OnCustomLeftClicked();
	UFUNCTION(BlueprintCallable)
	void OnCustomRightClicked();

	UFUNCTION(BlueprintCallable)
	void OnExitClicked();
	
	// Custom
	UFUNCTION(BlueprintCallable)
	void OnHairButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnFaceButtonClicked();
	UFUNCTION(BlueprintCallable)
	void OnTopButtonClicked();

protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerClassInfo_Widget;

	UPROPERTY()
	class UClassInfo_Widget* PlayerClassInfo_Instance = { nullptr };

protected : // 머터리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<class UMaterialInterface*> HairMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<class UMaterialInterface*> SkinMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<class UMaterialInterface*> TobMaterials;

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> SelectJob_Text;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Main_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Custom_Canvas;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Hair_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Top_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Skin_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Button_Exit;
	
private :
	TObjectPtr<class UCanvasPanel> Current_Canvas;
	TObjectPtr<class UButton> CheckButton;
	TObjectPtr<class UButton> ClickButton;

	int							Custom_Cnt = { 0 };
	TArray<UButton*>	HairButtonArray;
	TArray<UButton*>	FaceButtonArray;
	TArray<UButton*>	TopButtonArray;

	CUSTOMIZE_PLAYER CurrentCustom = { CUSTOMIZE_PLAYER::END_CUSTOM };

public :
	TObjectPtr<class AR1Player> SelectPlayer = { nullptr };
};
