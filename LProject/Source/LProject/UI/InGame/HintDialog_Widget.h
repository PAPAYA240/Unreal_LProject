// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/R1UserWidget.h"
#include "HintDialog_Widget.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API UHintDialog_Widget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UHintDialog_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnLetGo_Clicked();

protected :
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock>	Content_Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextBox")
	FString		TextBox;


	
};
