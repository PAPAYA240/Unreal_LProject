// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/R1Define.h"
#include "R1UserWidget.generated.h"


UCLASS()
class LPROJECT_API UR1UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UR1UserWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public : /* For. Function */
	// 이미지 변경
	void				ChangeImage(class UImage* ImageWidget, class UTexture2D* NewTexture);
	FVector2D		GetImagePosition(UImage* _image);
	FVector2D		GetImageSize(class UImage* _Image);
	void				SetImageSize(UImage* _Image, FVector2D _size); // 요건 캔버스를 통한 이미지 사이즈
	void				SetImageSize_Outward(UImage* _Image, FVector2D _size); // 요건 이미지 내부 외관 
	void				MoveImage(UImage* _image, FVector2D NewPosition);

	// 텍스트 변경
	FVector2D		GetTextPosition(class UTextBlock* _text);
	void				MoveText(class UTextBlock* _text, FVector2D NewPosition);

	// 버튼 변경
	FVector2D		GetButtonSize(class UButton* _button);
	void				SetButtonSize(class UButton* _button, FVector2D _size);
	void				ChangeButtonImage(class UButton* _buttonWidget, class UTexture2D* NewTexture);

	// 부가적인 Widget 기능
	FVector2D		RandomShake(FVector2D CurrentPosition, float ShakeAmount);
	FVector2D		GetWindowSize();

public :
	ESlateVisibility			Get_ShowWidget();
	void						Set_ShowWidget(bool _show);

	bool						Get_Exit() { return bExit; }
	void						Set_Exit(bool _exit) { bExit = _exit; }

	FVector2D				VInterpTo2D(FVector2D CurrentPos, FVector2D TargetPos, float DeltaTime, float InterpSpeed);
	
	class UCanvasPanel*	Get_Canvas() { return Canvas; }

private :
	bool						Find_Player();


protected :
	TObjectPtr<class AR1Player> R1Player							= { nullptr };
	TObjectPtr<class AMyPlayerState> CurrentPlayerState		= { nullptr };

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Canvas;

	
	UPROPERTY()
	bool bExit = { false }; // Widget 닫을 때 시간 텀을 두기 위하여

protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* ButtonSound;

	bool bParentTick = { true }; // 부모 틱을 돌릴 것인가?'
	FVector2D Origin_Position = {};
};
