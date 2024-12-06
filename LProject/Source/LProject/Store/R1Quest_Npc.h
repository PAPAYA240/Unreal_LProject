
#pragma once

#include "CoreMinimal.h"
#include "Store/R1Npc.h"
#include "R1Quest_Npc.generated.h"


UCLASS()
class LPROJECT_API AR1Quest_Npc : public AR1Npc
{
	GENERATED_BODY()

public:
	AR1Quest_Npc();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void			OnDialogueEnd(); // 이야기를 끝맞쳤을 때
	void			End_ContextTiming();

private  :
	bool Find_Widget();
	void Change_QuestImage();
	void Load_Npc();

public : // Getter Setter 
	bool Get_TargetQuestNpc() { return bTarget_QuestNpc;  }

protected:
	// 0 : 3D Widget
	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UWidgetComponent* CurrentState_Widget;
	UPROPERTY()
	TObjectPtr<class UUserWidget> WBPCurrent_Instance = { nullptr };

	// 1 : Dialog Widget
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Village_Widget;


	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UDialogEvent_Widget> Village_Instance;

	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> Target_QuestImage;

	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> Origin_QuestImage;

	TObjectPtr<class UMission_Widget> MissionWidget;

protected :
	UPROPERTY()
	int32				NextCnt						= { 0 }; // 다음 Dialog로 넘어가는 변수
	UPROPERTY()
	bool				bSpeak						= { true };
	UPROPERTY()
	bool				bTarget_QuestNpc			= { false };
	UPROPERTY()
	bool				bPrevTarget_QuestNpc	= { false };
	UPROPERTY()
	bool				bChange_Image			= { false };
};
