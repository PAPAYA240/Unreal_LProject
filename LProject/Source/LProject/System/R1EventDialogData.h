
#pragma once

#include "CoreMinimal.h"
#include "Define/R1Define.h"
#include "Engine/DataAsset.h"
#include "R1EventDialogData.generated.h"


USTRUCT()
struct FEventDialogData_Asset {
	GENERATED_BODY()

	// 빌딩 타입
	UPROPERTY(EditAnywhere, Category = Base)
	EBUILDING_TYPE Building_Type;

	// 빌딩 이미지
	UPROPERTY(EditAnywhere, Category = Base)
	TObjectPtr<UTexture2D> BuildingTexture;

	// 빌딩 이름
	UPROPERTY(EditAnywhere, Category = Base)
	FString BuildingName;

	// 빌딩 설명
	UPROPERTY(EditAnywhere, Category = Base)
	FString BuildingDescription;

	// 빌딩 서브 설명
	UPROPERTY(EditAnywhere, Category = Base)
	FString BuildingSubDescription;

	// 빌딩 버튼 설명
	UPROPERTY(EditAnywhere, Category = Base)
	FString Building_ButtonDescription;
};

UCLASS(BlueprintType)
class LPROJECT_API UR1EventDialogData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UR1EventDialogData();

	// 기본 자산 ID 가져오기
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("MMEventDialogData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere)
	TArray<FEventDialogData_Asset> AssetEventDialogs;

};
