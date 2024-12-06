// Fill out your copyright notice in the Description page of Project Settings.


#include "R1AseetData.h"
#include "UObject/ObjectSaveContext.h"
#include "Define/R1Define.h"
/*PDA를 저장하는 순간 사용한다
PDA : PDA는 주로 게임 내에서의 UI 또는 인터페이스 요소로 사용될 가능성이 높습니다*/
/* 최적화. */
/*PDA를 저장하는 타이밍에 걸린다. */
void UR1AssetData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	AssetNameToPath.Empty();
	AssetLabelToSet.Empty();
	AssetGroupNameToSet.KeySort([](const FName& A, const FName& B)
		{
			return (A.Compare(B) < 0);
		});

	for (const auto& Pair : AssetGroupNameToSet)
	{
		const FAssetSet& AssetSet = Pair.Value;
		for (FAssetEntry AssetEntry : AssetSet.AssetEntries)
		{
			FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
			/*const FString& AssetName = AssetPath.GetAssetName();
			if (AssetName.StartsWith(TEXT("BP_")) || AssetName.StartsWith(TEXT("B_")) || AssetName.StartsWith(TEXT("WBP_")) ||
				AssetName.StartsWith(TEXT("GE_")) || AssetName.StartsWith(TEXT("GA_")) || AssetName.StartsWith(TEXT("ABP_")))
			{
				FString AssetPathString = AssetPath.GetAssetPathString();
				AssetPathString.Append(TEXT("_C"));
				AssetPath = FSoftObjectPath(AssetPathString);
			}*/

			AssetNameToPath.Emplace(AssetEntry.AssetName, AssetEntry.AssetPath);
			for (const FName& Label : AssetEntry.AssetLabels)
			{
				AssetLabelToSet.FindOrAdd(Label).AssetEntries.Emplace(AssetEntry);
			}
		}
	}
}

FSoftObjectPath UR1AssetData::GetAssetPathByName(const FName& AssetName)
{

	FSoftObjectPath* AssetPath = AssetNameToPath.Find(AssetName);
	ensureAlwaysMsgf(AssetPath, TEXT("Can't find Asset Path from Asset Name [%s]."), *AssetName.ToString());
	return *AssetPath;
}

const FAssetSet& UR1AssetData::GetAssetSetByLabel(const FName& Label)
{
	const FAssetSet* AssetSet = AssetLabelToSet.Find(Label);
	ensureAlwaysMsgf(AssetSet, TEXT("Can't find Asset Set from Label [%s]."), *Label.ToString());
	return *AssetSet;
}
