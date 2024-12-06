#include "R1GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "System/R1GameInstance.h"

#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"
#include "Player/Player_Lumberjack/R1Player_Lumberjack.h"
#include "Player/Player_Herblist/R1Player_Herbalist.h"
#include "Player/Player_Wizard/R1Player_Wizard.h"

#include "Camera/PlayerCameraManager.h"
#include "Interactive_Buildings/ReconTower/R1ReconTower.h"
#include "Environment/Environment_Cloud.h"

#include "Monster/R1Monster.h"


AR1GameModeBase::AR1GameModeBase()
    : Super()
{
    PrimaryActorTick.bCanEverTick = true;

    //DefaultPawnClass = AR1Player::StaticClass();
    //PlayerStateClass = AMyPlayerState::StaticClass();
    PlayerControllerClass = APlayerController::StaticClass();
}

void AR1GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // Create to Player
    UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (!GameInstance)
        return;

    bool bPrevLobby = { false };
    // 1. PlayerState 생성
    for (int i = 0; i < LAST_PLAYER; i++)
    {
        AMyPlayerState* MyState = GetWorld()->SpawnActor<AMyPlayerState>(AMyPlayerState::StaticClass());
        int32 MonsterNumber = { 999 };
        if (MyState)
        {
            if (GameInstance->Load_Players.IsValidIndex(i))
            {
                // Player 정보 로드
                MyState->Player_CurrentScene = GameInstance->Load_Players[i].Player_CurrentScene;
                MyState->Player_PrevScene = GameInstance->Load_Players[i].Player_PrevScene;
                MyState->Player_Job = GameInstance->Load_Players[i]._JobType;
                MyState->Target_MonsterType = GameInstance->Load_Players[i].Target_MonsterType;
                MyState->LastPlayerPosition = GameInstance->Load_Players[i].LastPlayerPosition;
                MyState->bNextTarget = GameInstance->Load_Players[i].bTarget_Player;
                MyState->Current_DeadLine = GameInstance->Load_Players[i].Current_DeadLine;

                if (true == GameInstance->Load_Players[i].bDead)
                    MyState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)] = true;

                 for (int j = 0; j < sizeof(GameMode->PlayerStates[i]->Registration_Material) / sizeof(GameMode->PlayerStates[i]->Registration_Material[0]); j++)
                     MyState->Registration_Material[j] = GameInstance->Load_Players[i].Registration_Material[j];

                GameMode->PlayerStates[i] = MyState;

                if (ESCENE::LOBBY == MyState->Player_PrevScene)
                {
                    bPrevLobby = true;
                }
            }
            else
            {
                MSG_BOX(TEXT("R1GameModeBase() : 플레이어 정보를 가져오지 못했습니다."));
            }
        }
    }

    // 2. 캐릭터 생성
    if (ESCENE::INGAME == GameInstance->Load_Players[0].Player_CurrentScene
        || ESCENE::BOSS_SCENE == GameInstance->Load_Players[0].Player_CurrentScene)
    {
        int32 iPlayerCnt = { 0 };
        for (auto iter : GameInstance->Load_Players)
        {
            AR1Player* PlayerCharacter = Create_Player(iter._JobType, FVector::ZeroVector, FRotator::ZeroRotator);

            // 만약 로비 -> 인게임이 아니라면,
            if(false == bPrevLobby)
            {
                PlayerCharacter->TargetHill = PlayerCharacter->FinalHill = GameInstance->Load_Players[iPlayerCnt].Hill;
                PlayerCharacter->CurrentHp = PlayerCharacter->Hp = GameInstance->Load_Players[iPlayerCnt].Hp;
                PlayerCharacter->TargetMaxHp = PlayerCharacter->MaxHp = GameInstance->Load_Players[iPlayerCnt].MaxHp;
                PlayerCharacter->TargetAttack = PlayerCharacter->CurrentAttack = GameInstance->Load_Players[iPlayerCnt].CurrentAttack;
            }
            ++iPlayerCnt;
        }
    }

    else if (ESCENE::FIGHT_SCENE == GameInstance->Load_Players[0].Player_CurrentScene)
    {
        int32 iPlayerCnt = { 0 };
        for (auto iter : GameInstance->Load_Players)
        {
            if(true == iter.bTarget_Player)
            {
                AR1Player* PlayerCharacter = Create_Player(iter._JobType, FVector{ -240.f, 0.f , 100.f }, FRotator{ 0.f, 180.f, 0.f });

                PlayerCharacter->TargetHill = PlayerCharacter->FinalHill = GameInstance->Load_Players[iPlayerCnt].Hill;
                PlayerCharacter->CurrentHp = PlayerCharacter->Hp = GameInstance->Load_Players[iPlayerCnt].Hp;
                PlayerCharacter->TargetMaxHp = PlayerCharacter->MaxHp = GameInstance->Load_Players[iPlayerCnt].MaxHp;
                PlayerCharacter->TargetAttack = PlayerCharacter->CurrentAttack = GameInstance->Load_Players[iPlayerCnt].CurrentAttack;
                break;
            }
            ++iPlayerCnt;
        }
    }
}
    
void AR1GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
   
    // 1. 몬스터 넘버 지정하기
    if (false == bSetting_Number)
    {
        int32 iMonsterNum = { 0 };
        int32 iCloudNum = { 0 };
        UObject* WorldContextObject = GetWorld();
        UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

        // 1. Monster
        TArray<AR1Monster*> MonsterArray;
        MonsterArray = AR1Player::FindActorsByClass<AR1Monster>(WorldContextObject);
        for (auto iter : MonsterArray)
        {
            iter->Set_MonsterNumber(iMonsterNum);

            if (ESCENE::INGAME == CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene)
            {
                if (GameInstance->DestroyMonster_Number.Num() > 0)
                {
                    TArray<AR1ReconTower*> ReconTower_Array = AR1Player::FindActorsByClass<AR1ReconTower>(WorldContextObject);
                    for (auto iter2 : GameInstance->DestroyMonster_Number)
                    {
                        if (iter2 == iMonsterNum)
                        {
                            for (auto Recon_iter : ReconTower_Array)
                            {
                                if (Recon_iter->Get_ReconTowerType() == iter->Get_Monster_RegionType())
                                {
                                    Recon_iter->PercentChange();
                                }
                            }
                            iter->Destroy();
                        }
                    }
                }
            }
            ++iMonsterNum;
        }

       // 2. Cloud
        TArray<AEnvironment_Cloud*> CloudArray;
        CloudArray = AR1Player::FindActorsByClass<AEnvironment_Cloud>(WorldContextObject);
        for (auto iter : CloudArray)
        {
            iter->Set_CloudNumber(iCloudNum);
            if (ESCENE::INGAME == CurrentPlayer->Get_CurrentPlayerState()->Player_CurrentScene)
            {
                if (GameInstance->DestroyCloud_Number.Num() > 0)
                {
                    for (auto iter2 : GameInstance->DestroyCloud_Number)
                    {
                        if (iCloudNum == iter2)
                        {
                            iter->Destroy();
                            break;
                        }
                    }
                }
            }
            ++iCloudNum;
        }

        bSetting_Number = true;
    }
}

void AR1GameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AR1GameModeBase::SwitchPlayer()
{
}

AR1Player* AR1GameModeBase::Create_Player(EJOB_TYPE _job, FVector _spawnLoc, FRotator _spawnRot)
{
    AR1Player* PlayerCharacter  = { nullptr };
    switch (_job)
    {
    case EJOB_TYPE::LUMBERJACK:
    {
        UClass* PlayerBPClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/Blueprints/Player/BP_R1Player.BP_R1Player_C'"));
        if (PlayerBPClass)
        {
            PlayerCharacter = GetWorld()->SpawnActor<AR1Player_Lumberjack>(PlayerBPClass, _spawnLoc, _spawnRot);
        }
    }
    break;

    case EJOB_TYPE::WIZARD:
    {
        UClass* PlayerBPClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/Blueprints/Player/BP_R1Player3.BP_R1Player3_C'"));
        if (PlayerBPClass)
        {
            PlayerCharacter = GetWorld()->SpawnActor<AR1Player_Wizard>(PlayerBPClass, _spawnLoc, _spawnRot);
        }
    }
    break;

    case EJOB_TYPE::HERBALIST:
    {
        UClass* PlayerBPClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/Blueprints/Player/BP_R1Player2.BP_R1Player2_C'"));
        if (PlayerBPClass)
        {
            PlayerCharacter = GetWorld()->SpawnActor<AR1Player_Herbalist>(PlayerBPClass, _spawnLoc, _spawnRot);
        }
    }
    break;

    default:
        break;
    }

    return PlayerCharacter;
}

