

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "PlayerBar_Widget.generated.h"

/* Class : 플레이어 Turn Bar */
UCLASS()
class LPROJECT_API UPlayerBar_Widget : public UR1UserWidget
{
	GENERATED_BODY()
	
public :
	UPlayerBar_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public : /* For. Getter Setter */
	TArray<FChaosInfo*>					Get_Chaos_Information() { return Chaos_Map; }
	void										Load_Chaos_Information();

	int											Get_TurnCnt() { return TurnCnt;  }

	void										Set_FightType(EFIGHT_AUTHORITY _type);
	EFIGHT_AUTHORITY					Get_FightType() { return Fight_AuthorityType; }

	void										Set_NextTurnType(bool _next = true) { bNextCharacter = _next; }

	TArray<class AR1Character*>		Get_NextCharacterType_Array() { return FightSecond_Array; }

	void										MoveChaosAndNextTurn(bool _start) { bNextTurn = _start; }
	void										MonsterDead_ChangeTurn();

	// Delete Chaos : 카오스 완화 
	bool										Destory_Chaos();

protected : /* For. Setting Function */
	bool					Setting_Widget();
	bool					Setting_Monsters();
	bool					Setting_WidgetArray();

protected : /* For. Operate Scene*/
	void					Operate_InGame();
	void					Operate_FightScene();

protected : /* For. Function InGame 1 */
	UFUNCTION(BlueprintCallable)
	void					OnOption_Clicked();
	UFUNCTION(BlueprintCallable)
	void					OnTurnChange_Clicked();

	void					ShowWaring();
	void					Stop_ShowWaring();
	void					PlayerStart_Setting();
	void					Move_Chaos(float DeltaTime);
	// 플레이어의 목숨 : 떨어질 때 라이프가 깎인다
	bool					Change_PlayerLife();

private : /* For. Function InGame 2 */
	void					Show_PlayerBarType();
	void					SetOpacity(class UCanvasPanel* _canvas, float _opercity);
	float					GetImageXPosition(int32 Index); // Turn Iange의 X 축 가져오기
	FVector2D			GetImageScale(int32 Index); // 스케일 가져오기
	void					Setting_NextTurn(FVector2D NewLocation, FChaosInfo* _chaosInfo); // 카오스 다음 턴으로 움직이기
	FVector2D			GetImageLocation(class UImage* Image);
	

private: /* For. Function Fight Scene */
	void					FightTurn_Manager();
	void					ChoasMoving_Warning();

protected : /* For. Widget Variable */
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage> Chaos;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> TurnCanvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Warning_Canvas;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> ChoasWarning_Canvas;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Chaos_Collection_Canvas;

private : /* In Game*/
	// 회차 박스 모음
	TArray<class UImage*> TurnBox_Array;
	// 카오스 적립 모음
	TArray<class UImage*> Chaos_Array;

	TArray<FChaosInfo*>		Chaos_Map;
	// 라이프 적림 모음
	TArray<class UImage*> Life_Array;

	UPROPERTY()
	bool							bNextTurn = { false };
	UPROPERTY()
	int32							MonsterTurn_Cnt	= { 12 }; // 0 ~ 12
	UPROPERTY()
	int32							Chaos_Cnt = { 0 };

	UPROPERTY()
	float							OpercityValue = { 1.f };
	UPROPERTY()
	float							ShowWaring_Timer = { 0.f };
	UPROPERTY()
	bool							bShowWaring = { false };

	UPROPERTY()
	FVector2D					Canvas_InitPos;

	// Chaos Warning
	UPROPERTY()
	float							ChaosWarning_Timer = { 0.f };
	UPROPERTY()
	bool							bChaosWarning = { false };
	UPROPERTY()
	bool							bUpperOpacity = { false };

	// Life
	UPROPERTY()
	int32							Last_DeadLineCnt = { 0 };
	UPROPERTY()
	int								TurnCnt = { 0 }; // 현재 게임 턴은 몇 번 째 인가?
	UPROPERTY()
	int								AppearanceOfChaos_Turn = { 2 }; // 카오스 생성 텀

	UPROPERTY()
	bool							bLoadWidget			= { false };
private : /* In Fight*/
	UPROPERTY()
	EFIGHT_AUTHORITY				Fight_AuthorityType = { EFIGHT_AUTHORITY::PLAYER };
	// Fight 순서
	UPROPERTY()
	TArray<class AR1Character*>	FightSecond_Array;

	

	UPROPERTY()
	bool									bNextCharacter = { false };
};
