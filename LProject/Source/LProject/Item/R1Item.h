// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Define/R1Define.h"
#include "R1Item.generated.h"

UCLASS()
class LPROJECT_API AR1Item : public AActor
{
	GENERATED_BODY()
	
public:	
	AR1Item();

protected:
	virtual void BeginPlay() override;

protected :
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent>  SphereComponent;

public:
	// 아이템 타입
	UPROPERTY(EditAnywhere, Category = Base)
	EITEM_TYPE ItemType;

	// 아이템 이미지
	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> ItemTexture;

	// 아이템 이름
	UPROPERTY(EditAnywhere, Category = Base)
	FString ItemName;

	// 아이템 구매 가격
	UPROPERTY(EditAnywhere, Category = Base)
	int32 ItemPurchasePrice;

	// 아이템 판매 가격
	UPROPERTY(EditAnywhere, Category = Base)
	int32 ItemSalePrice;

	// 아이템 설명서
	UPROPERTY(EditAnywhere, Category = Base)
	FString ItemDocument;

	// 파는 상인
	UPROPERTY(EditAnywhere, Category = Base)
	ENPC_TYPE MerchantType;
};
