#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "R1AseetData.generated.h"


USTRUCT()
struct FAssetEntry
{
	GENERATED_BODY()

public :
	UPROPERTY(EditDefaultsOnly)
	FName AssetName;

	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath AssetPath;

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AssetLabels;

};

USTRUCT()
struct FAssetSet
{
	GENERATED_BODY()

public :
	UPROPERTY(EditDefaultsOnly)
	TArray<FAssetEntry> AssetEntries;
};

UCLASS()
class LPROJECT_API UR1AssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public :
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

public : /* 이름으로 찾거나 라벨로 찾거나*/
	FSoftObjectPath GetAssetPathByName(const FName& AssetName);
	const FAssetSet& GetAssetSetByLabel(const FName& Label);
	// 라벨 이름을 가지고 올 수 있도록 하는 함수

private :
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetSet> AssetGroupNameToSet;
	// C++의 Map

	UPROPERTY()
	TMap<FName, FSoftObjectPath> AssetNameToPath;

	UPROPERTY()
	TMap<FName, FAssetSet> AssetLabelToSet;

};
