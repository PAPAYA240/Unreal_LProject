// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "RestArea_Widget.generated.h"


UCLASS()
class LPROJECT_API URestArea_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public :
	URestArea_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private :
	UFUNCTION()
	void OnServiceButtonClick(); // 서비스

	UFUNCTION()
	void OnQuestButtonClick();	 // 퀘스트

	UFUNCTION()
	void OnStoreButtonClick();	// 시장

	UFUNCTION()
	void OnExitButtonClick();		// 떠나기

	UFUNCTION()
	void OnQuestExitButtonClick();	// 퀘스트 창 닫기

	UFUNCTION()
	void OnStoreExitButtonClick();	// 시장 창 닫기

protected:
	// 선택 창 캔버스
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Select_Canvas;

	// 서비스 캔버스
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Service_Canvas;

	// 퀘스트 창 캔버스
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Quest_Canvas;

	// 시장 창 캔버스
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Store_Canvas;


};
