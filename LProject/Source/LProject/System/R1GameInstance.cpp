#include "R1GameInstance.h"
#include "R1AssetManager.h"

UR1GameInstance::UR1GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR1GameInstance::Init()
{
	Super::Init();

	UR1AssetManager::Initialize();

	for(int i = 0; i < LAST_PLAYER; i++)
	{
		AR1LoadPlayer::Load_PlayerInform NewPlayer = {};
		Load_Players.Emplace(NewPlayer);
	}
}

/* 클래스의 종료 혹은 정리 작업을 수행하는 데 사용됩니다. 예를 들어, 자원 해제, 메모리 정리, 연결 종료 등과 같은 작업이 포함될 수 있습니다. */
void UR1GameInstance::Shutdown()
{
	Super::Shutdown();
}

void UR1GameInstance::Set_LoadPlayer(AR1LoadPlayer::Load_PlayerInform _player)
{
	AR1LoadPlayer::Load_PlayerInform _savePlayer;

	_savePlayer._JobType = _player._JobType;

	if (Load_Players.Num() > 3)
		Load_Players.Empty();

	Load_Players.Emplace(_savePlayer);
}
