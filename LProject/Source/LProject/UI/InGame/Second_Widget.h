// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "Second_Widget.generated.h"


UCLASS()
class LPROJECT_API USecond_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	USecond_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Set_SecondWidget_Start(bool _start = true)		{ bGameStart = _start; }
	bool Get_SecondWidget_Start()							{ return bGameStart; }// 초를 기다리는 동안
	bool Get_MouseMoving()									{ return bMouseMoving; }
	void Set_StopMouseMoving()								{ bMouseMoving = false; }

	bool Second_Configure(); // 초기화 구간

	bool TurnEndZoomTransition(); // 턴 완료를 위한 줌인줌아웃


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

protected : /* For. Moving */
	void Random_StartSetting();

	bool Random_Second(float DeltaTime);
	bool Random_Move(float DeltaTime);
	bool Ready_Moving(float DeltaTime);
	void Start_Moving(float DeltaTime);

	void Distinction_Ability(); // 턴이 끝났을 때 플레이어 능력을 사용할 것인가?
	bool Use_Ability();

protected : // For. Scene First Story 함수
	UFUNCTION()
	void OnContinueButtonClicked();
	void ChangeChildWidgetsColorToBlack();

private : // For. Setting 
	bool			Setting_Widget();
	bool			Setting_Position();

protected: // For. Widget 
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		Second_Text;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		Target_SecondText;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		Guidance_Text;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>		Moving_Ready_Text;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage>			Second_BackGround;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UImage>			Target_BackGround;
	

protected :
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> FirstSceneStory_Widget;
	UPROPERTY()
	UUserWidget* FirstSceneStory_Instance = { nullptr };

private :
	FVector2D Init_ImagePos;
	FVector2D Init_TextPos;

	FVector2D Target_ImagePos;
	FVector2D Target_TextPos;

private :
	UPROPERTY()
	bool bGameStart = { false };
	UPROPERTY()
	bool bGameReady = { false };
	UPROPERTY()
	bool bMovingStart = { false };
	UPROPERTY()
	bool bZoomIn = { false };
	UPROPERTY()
	bool bMouseMoving = { false }; // ** 다음 턴 시작이 가능함을 알림
	UPROPERTY()
	bool bInitCamer_Setting = { false };

	UPROPERTY()
	float Random_Timer = { 0.f };
	UPROPERTY()
	float UpdateInterval = 0.5f;
	UPROPERTY()
	float MoveingReady_Timer = 0.f;
	UPROPERTY()
	float SinceLastUpdateTimer = 0.f;
	UPROPERTY()
	float MovingStart_Timer = 0.f;
	UPROPERTY()
	float RandomTime = { 0 };
	UPROPERTY()
	bool bPlayer_TurnAbility = { false }; // Player 능력 사용 중
	UPROPERTY()
	bool bUseAbility = { false }; // Player 능력 사용 시작
	// Scene First Story 변수
	bool bUseAbility_Distinction = { false }; // 능력을 사용할 지 판단했는가?
private :
	bool bFirstSceneEnd = { false };
	float ColorChange_Timer = { 1.f };
	bool bBlackColor = { false };
};
