#pragma once

#include "Engine/AssetManager.h"
#include "R1AseetData.h"
#include "Define/R1Define.h"
#include "R1AssetManager.generated.h"

/**
에셋을 관리하는 역할, 
들고 있는 모든 에셋을 관리한다. ( 싱글톤 ) 
 */
DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName&/*AssetName or Label*/, UObject*/*LoadedAsset*/);

// FAsyncLoadCompletedDelegate의 변수를 받을 수 있음
UCLASS()
class LPROJECT_API UR1AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public :
	UR1AssetManager();
	virtual void StartInitialLoading() override;

	static UR1AssetManager& Get(); /* 싱글톤으로 만들어 주기 */

public :
	template<typename AssetType>
	static AssetType* GetAssetByName(const FName& AssetName);

public :
	static void Initialize();

	/* 비동기 동기 로딩을 결정해야 하는데 
	Sync라는 버전이 붙으면 동기
	Async라는 버전이 붙으면 비동기 방식이다. 
	
	비동기 방식 : 콜링 방식 : 우리가 완료가 되었는 지 매번 체크or
	시스템 상으로 콜백 함수가 호출이 되어가지고 완료가 되었다는 통지가 거꾸로 호출 
	=> 함수 포인터 :std::function => delegate
	*/
	static void LoadSyncByPath(const FSoftObjectPath& AssetPath);
	static void LoadSyncByName(const FName& AssetName);
	static void LoadSyncByLabel(const FName& Label);

	static void ReleaseByPath(const FSoftObjectPath& AssetPath);
	static void ReleaseByName(const FName& AssetName);
	static void ReleaseByLable(const FName& Label);
	static void ReleaseAll();

private :
	void AddLoadedAsset(const FName& AssetName, const UObject* Asset);
	void LoadPreloadAssets();

private :
	UPROPERTY()
	TObjectPtr<UR1AssetData> LoadedAssetData;
	/* 로드한 Asset들을 여기에 들고 있을 예정이다.*/

	UPROPERTY()
	TMap<FName, TObjectPtr<const UObject>> NameToLoadedAsset;
	/* 에셋을 캐싱하는 부분을 여기에 넣어줄 예정이다. */
};

template<typename AssetType>
inline AssetType* UR1AssetManager::GetAssetByName(const FName& AssetName)
{
	UR1AssetData* AssetData = Get().LoadedAssetData;

	if (nullptr == AssetData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Failed Create Asset :  [%s]"), *AssetName.ToString()));
		MSG_BOX(TEXT("NONON Input"))

		return nullptr;
	}
	check(AssetData);

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);

	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());

		if (LoadedAsset == nullptr)
		{
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}

	return LoadedAsset;
}
