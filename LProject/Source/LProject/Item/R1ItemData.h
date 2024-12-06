
#pragma once

#include "CoreMinimal.h"
#include "Define/R1Define.h"
#include "Engine/DataAsset.h"
#include "R1ItemData.generated.h"

USTRUCT()
struct FItemData_Asset {
	GENERATED_BODY()

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

	// 이 아이템을 가지고 있을 수 있는 몬스터
	UPROPERTY(EditAnywhere, Category = Base)
	EMONSTER_TYPE	MonsterType = EMONSTER_TYPE::END;
};

UCLASS(BlueprintType) 
class LPROJECT_API UR1ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UR1ItemData();

	// 기본 자산 ID 가져오기
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MMItemData", GetFName());
	}


public:
	UPROPERTY(EditAnywhere)
	TArray<FItemData_Asset> AssetItems;


public : // 아이템 기능


};
