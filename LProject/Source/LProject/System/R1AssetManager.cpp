#include "R1AssetManager.h"

UR1AssetManager::UR1AssetManager() : Super()
{
	MSG_BOX(TEXT("Asset Manager Create"))
}

void UR1AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 자산 로드 코드 추가
	// 예: LoadedAssetData = LoadAssetData("/Game/Path/To/InputData");

	if (LoadedAssetData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("LoadedAssetData initialized successfully."));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Failed to initialize LoadedAssetData."));
	}
}

UR1AssetManager& UR1AssetManager::Get()
{
	if (UR1AssetManager* Singleton = Cast<UR1AssetManager>(GEngine->AssetManager))
	{
		return *Singleton;  
	}

	return *NewObject<UR1AssetManager>();
}

void UR1AssetManager::Initialize()
{
	// TODO : Asset Load
	Get().LoadPreloadAssets();
}
/* */
void UR1AssetManager::LoadSyncByPath(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid()) /* Pointer가 유효한지 확인한다. */
	{
		/* 혹시나 이미 메모리적으로 들고 있는지? 로딩했는지? 확인한다. */
		UObject* LoadedAsset = AssetPath.ResolveObject();

		if (nullptr == LoadedAsset) /* 없다면 Try Load*/
		{
			if (UAssetManager::IsInitialized())
			{
				/* 동기 : 여기서 호출이 끝날 때까지 대기한다. */
				/* 비동기 : 일단 진행하고 나중에 끝났다는 호출이 오면 그때 처리할 수 있다.
				유연하지만, 코드가 복잡해진다. */
				LoadedAsset = UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
			}
			else
			{	
				LoadedAsset = AssetPath.TryLoad();
			}
		}
		if (LoadedAsset)
		{
			/*GetAssetName : 반환 FString, GetAssetFName :  반환 FName
			const FName 참조 시에 *GetAssetName or GetAssetFName */
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Create to load asset from path: [%s]"), *AssetPath.ToString()));

			/* Loading 한 객체를 요 이름으로 등록하겠다. */
		}
		else
		{

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Failed to load asset from path: [%s]"), *AssetPath.ToString()));
		}
	}
}

void UR1AssetManager::LoadSyncByName(const FName& AssetName)
{
	UR1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if(nullptr == AssetData)
		MSG_BOX(TEXT("OMGGGGGG"))

	LoadSyncByPath(AssetPath);
}

/* Tag가 붙어있는 것들을 싹다 불러온다. */
void UR1AssetManager::LoadSyncByLabel(const FName& Label)
{
	if (UAssetManager::IsInitialized() == false)
	{
		return;
	}

	UR1AssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	TArray<FSoftObjectPath> AssetPaths;

	const FAssetSet& AssetSet = AssetData->GetAssetSetByLabel(Label);
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		LoadSyncByPath(AssetPath);
		if (AssetPath.IsValid())
		{
			AssetPaths.Emplace(AssetPath);
		}
	}

	GetStreamableManager().RequestSyncLoad(AssetPaths);

	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;

		if (AssetPath.IsValid())
		{
			if (UObject* LoadedAsset = AssetPath.ResolveObject())
			{
				Get().AddLoadedAsset(AssetEntry.AssetName, LoadedAsset);
			}
			else
			{

			}
		}
	}
}

void UR1AssetManager::ReleaseByPath(const FSoftObjectPath& AssetPath)
{
}

void UR1AssetManager::ReleaseByName(const FName& AssetName)
{
}

void UR1AssetManager::ReleaseByLable(const FName& Label)
{
}

void UR1AssetManager::ReleaseAll()
{
}

/* 로딩한 걸 캐싱하는 부분*/
void UR1AssetManager::AddLoadedAsset(const FName& AssetName, const UObject* Asset)
{
	if (AssetName.IsValid() && Asset)
	{
		//FScopeLock LoadedAssetsLock(&LoadedAssetsCritical); 
		// 멀티 스레드 대응, Lcok

		if (NameToLoadedAsset.Contains(AssetName) == false)
		{
			NameToLoadedAsset.Add(AssetName, Asset);
		}
	}
}

/* PreLoad Tag가 붙은 아이들은 처음에 로딩하고 시작하겠다는 함수*/
void UR1AssetManager::LoadPreloadAssets()
{
	if (LoadedAssetData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("LoadedAsset Data")));
		return;
	}

	UR1AssetData* AssetData = nullptr;
	FPrimaryAssetType PrimaryAssetType(UR1AssetData::StaticClass()->GetFName());
	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
	// LoadPrimaryAssetsWithType : Asset Type을 통해서 뭔가를 로딩하는 부분, 주 자산(Primary Asset)을 비동기적으로 로드하는 기능입니다.

	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete(0.f, false);
		AssetData = Cast<UR1AssetData>(Handle->GetLoadedAsset());
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Non Handle Vailed")));


	if (AssetData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Get Asset Data") ));

		LoadedAssetData = AssetData;
		LoadSyncByLabel("Preload");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("NON Get Asset Data")));
	}
}
