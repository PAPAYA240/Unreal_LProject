#include "Player/R1LoadPlayer.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "Kismet/GameplayStatics.h" 
#include "System/R1GameInstance.h"
#include "R1GameModeBase.h"


AR1LoadPlayer::AR1LoadPlayer()
	: Super()
{
}

void AR1LoadPlayer::BeginPlay()
{
	Super::BeginPlay();
}

bool AR1LoadPlayer::Save_Player()
{
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
		return false;
	Load_PlayerInform _player = {};

	_player._JobType = CurrentPlayerState->Player_Job;
	_player.Player_CurrentScene = CurrentPlayerState->Player_CurrentScene;
	_player.Player_PrevScene = CurrentPlayerState->Player_PrevScene;

	if (3 < GameInstance->Load_Players.Num())
		GameInstance->Load_Players.Empty();
	GameInstance->Load_Players.Emplace(_player);
	return true;
}

bool AR1LoadPlayer::Setting_Material()
{
	return false;
}

bool AR1LoadPlayer::Load_Completed()
{
	if (bLoadPlayer)
	{
		if (AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AR1Player* pLoadPlayer = Cast<AR1Player>(GameMode->CurrentPlayer);
			if (nullptr == pLoadPlayer)
				return false;

			// 2. 로딩이 끝났다면 턴을 넘김
			if (pLoadPlayer->End_Turn(true))
			{
				// End Player Load : 
				for (auto iter : GameMode->PlayerArray)
				{
					iter->Set_LoadPlayer(false);
				}
			}
		}
		else
			return false;
	}
	return true;
}
