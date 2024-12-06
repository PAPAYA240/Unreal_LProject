// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Store/R1Npc.h"
#include "R1Boss_Merchant.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AR1Boss_Merchant : public AR1Npc
{
	GENERATED_BODY()

public:
	AR1Boss_Merchant();
	virtual void Tick(float DeltaTime) override;

	void DestroyMerchant();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBossStoreClicked();
	UFUNCTION()
	void OnNextButtonClicked();
	UFUNCTION()
	void OnStartClicked();

	bool	ShowWidget();

private :
	bool		WidgetOption_Setting();

protected :
	// 4. BossMap
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> BossMapStore_Widget;

	UPROPERTY()
	class UR1UserWidget* BossMapStore_Instance = { nullptr };

	TObjectPtr<class UCanvasPanel> ReadyCanvas;
	TObjectPtr<class UCanvasPanel> Store_Canvas;

private :
	bool			bStoreOpen = { false };
	bool			bNext = { false };
	bool			bReadyStart = { false };

	float			AfterStart_Timer = { 0.f };
	
};
