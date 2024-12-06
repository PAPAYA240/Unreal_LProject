// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "ChallengeDialog_Widet.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UChallengeDialog_Widet : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UChallengeDialog_Widet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Set_BuildingType(EBUILDING_TYPE _building) { Building_Type = _building; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void OnChallengeButtonClick();

	bool UpdateEnergyImage(float DeltaTime);
	void Reset_Probability();
	void RamdomAbility_Change(bool _result);
	void Use_Energy(bool _using);

private :
	bool Find_DialogData();

public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* ChangeTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* InitTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* RandomInit_Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* RandomFailed_Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* RandomSucess_Texture;

protected :
	// 도전하기 버튼
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UButton> Challenge_Button;

	// 확률 이미지 캔버스
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> RandomResult_Canvas;

	// 에너지 캔버스
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> Energy_Canvas;
	
private : // Building
	EBUILDING_TYPE					Building_Type = { EBUILDING_TYPE::END_BUILDING };

private :
	UPROPERTY()
	TMap<class UImage*, bool> Energy_Map;

	UPROPERTY()
	bool  bChallenge = { false };

	UPROPERTY()
	float SaveTimer = { 0.f };

	UPROPERTY()
	int32 ResultCnt = { 0 };

	UPROPERTY()
	int32 RandomResult_Cnt = { 0 };

	UPROPERTY()
	bool bSettingData = { false };

	UPROPERTY()
	int32		ChallengeCnt = { 0 };
	UPROPERTY()
	float		ChallengeTimer = { 0.f };
};
