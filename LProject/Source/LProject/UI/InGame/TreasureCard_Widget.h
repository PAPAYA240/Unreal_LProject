
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "TreasureCard_Widget.generated.h"

UENUM(BlueprintType)
enum class ECARD_REWARD_TYPE : uint8
{
	BANG_CARD, // 꽝
	SIMPLE_CARD, // 소박
	MEDIUM_CARD, // 중박
	AWESOME_CARD,  // 대박
	END_CARD
};

UCLASS()
class LPROJECT_API UTreasureCard_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public :
	UTreasureCard_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

protected:
	UFUNCTION()
	void							OnClickCard();

private :
	bool							Setting_WidgetOption();
	bool							Select_RandomCardType();

	ECARD_REWARD_TYPE	GetRandomCardRewardType();

	void							Operate_SelectCard();
	void							Reset();

protected :
	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> Bang_Image;

	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> Simple_Image;

	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> Medium_Image;

	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> Awesome_Image;

	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> SelectBox_Image;

private :
	UPROPERTY()
	bool				bSelectCard							= { false }; // 카드 동작
	UPROPERTY()
	bool				bTurnUpSide						= { false };
	UPROPERTY()
	bool				bVisibleImage						= { false };

	UPROPERTY()
	FVector2D		OriginSize = {};

	UPROPERTY()
	TObjectPtr<class UButton>		Select_Button = { nullptr };

	UPROPERTY()
	float									SelectCard_Timer = { 0.f };

	TMap<class UButton*, ECARD_REWARD_TYPE>	Card_Map;

	TArray<ECARD_REWARD_TYPE>						SelectReward_Array;

};
