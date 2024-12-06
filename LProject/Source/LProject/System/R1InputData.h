// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Define/R1Define.h"
#include "R1InputData.generated.h"

USTRUCT()
struct FR1InputAction
{
	GENERATED_BODY()

public :
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInputAction> InputAction = nullptr;
};

UCLASS()
class LPROJECT_API UR1InputData : public UDataAsset
{
	GENERATED_BODY()

public :
	const UInputAction* FindInputActionByTag(const FGameplayTag& InputTag) const;

public :
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly) // 에디터에서 이것저것 설정하고 싶다,
	TArray<FR1InputAction> InputActions;
};
