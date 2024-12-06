
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "ClassInfo_Widget.generated.h"

UCLASS()
class LPROJECT_API UClassInfo_Widget : public UR1UserWidget
{
	GENERATED_BODY()
	
public:
	UClassInfo_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void Camera_Setting(bool _setting);
	bool Get_IsSelectPlayer() { return bSelectPlayer;  } // 플레이어를 선택하였는가?
	void Set_SelectPlayer(bool _select) { bSelectPlayer = _select;  }
	AR1Player* InitPos_Setting();

public :
	FText	Get_JobName();
	HoveringType Get_AbilityBox_Info() { return HoveringInfo; }
	void Get_PlayerSettingClass_Ptr(class UPlayerSetting_Widget* _pClass) { PlayerSetting_Instance = _pClass;  }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void CheckHoveringImages();
	void Change_SkeletalMesh();

private :
	UFUNCTION(BlueprintCallable)
	void OnLeftArrowClicked();

	UFUNCTION(BlueprintCallable)
	void OnRightArrowClicked();

	UFUNCTION(BlueprintCallable)
	void OnExitClicked();

	void Change_Job();
	bool Setting_Widget();

protected: /* Button*/
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> JobName;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Job_Info;


protected: // 스켈레탈 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
	TArray<class USkeletalMesh*> Job_SkeletalMesh;


protected : /* Ability*/
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> AbilityCanvas;

protected :
	UPROPERTY()
	TObjectPtr<class UCameraComponent> CameraComponent;

	// Widget
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> AbilityBox_Widget;

	UPROPERTY()
	class UAbilityInfo_Widget* AbilityBoxWidget_Instance = { nullptr };

	TObjectPtr<class UPlayerSetting_Widget> PlayerSetting_Instance = { nullptr };

private :
	bool				bSettingPos = { false };
	bool				bSelectPlayer = { false };

	EJOB_TYPE		 JobType = { EJOB_TYPE::END };
	HoveringType	 HoveringInfo;
	bool				bCallback		= { false };
	float				InterpSpeed		= { 2.f };
	FVector			Init_LocationCam = {};
	FVector			SelectPlayer_LocationCam = {};
};
