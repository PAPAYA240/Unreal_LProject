
#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "AbilityInfo_Widget.generated.h"

UCLASS()
class LPROJECT_API UAbilityInfo_Widget : public UR1UserWidget
{
	GENERATED_BODY()
	
public :
	UAbilityInfo_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public :
	void Set_AbilityBox_HoveringInfo(HoveringType _hoveringInfo)
	{
		HoveringInfo = _hoveringInfo;
	}
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);


private :
	void Chang_Info();
	void SetCanvasPosition();

protected :
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> AbilityName_Text;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> AbilityInfo_Text;


private :
	HoveringType HoveringInfo;
};
