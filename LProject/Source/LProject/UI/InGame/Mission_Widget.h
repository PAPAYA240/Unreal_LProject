

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "Mission_Widget.generated.h"


UCLASS()
class LPROJECT_API UMission_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UMission_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void			NativeConstruct() override;
	virtual void			NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public : // For. Getter Setter 
	// 미션 설정
	void					Setting_MissionType(EMISSION_TYPE _mission);
	
	// 현재 미션 타입
	EMISSION_TYPE	Get_CurrentMission() { return CurrentMissionType;  } 
	void					Set_CurrentMission(EMISSION_TYPE _mission) { CurrentMissionType = _mission; }
	bool					Get_MissionClear() { return bMissionClear; } // 미션 클리어 여부 확인

private :
	bool					Setting_WidgetOption();

	void					PlayerStart_Setting();
	void					ShowWidget();
	void					Mission_Condition();

protected:
	// 미션 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* MissionSound;

protected : 
	// 메인 퀘스트 이름
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Main_Text;

	// 메인 퀘스트 설명
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> Description_Text;


private :
	EMISSION_TYPE		CurrentMissionType		= { EMISSION_TYPE::VISIT_VILLAGEMAN };
	bool						bMissionStart				= { false };
	bool						bMissionClear				= { false };
	FVector2D				OriginPos;


};
