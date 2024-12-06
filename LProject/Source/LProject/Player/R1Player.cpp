#include "R1Player.h"
#include "Player/R1PlayerController.h"
#include "Player/MyPlayerState.h"
#include "Player/R1LoadPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

/* Instance*/
#include "System/R1GameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Monster
#include "Monster/R1Monster.h"
#include "Monster/R1Monster.h"
#include "Monster/R1Monster_Wizard.h"
#include "Monster/Bee/R1Monster_Bee.h"
#include "Monster/Chest/R1Monster_Chest.h"
#include "Monster/Mushroom/R1Monster_Mushroom.h"
#include "Monster/TurtleShell/R1Monster_TurtleShell.h"
#include "Monster/Swarm/R1Monster_Swarm.h" 
#include "Monster/Crap/R1Monster_Crap.h"
#include "Monster/LizardWarrior/R1Monster_LizardWarrior.h"
#include "Monster/Rat/R1Monster_Rat.h"
#include "Monster/Specter/R1Specter_Monster.h"
#include "Monster/Wolf/R1Monster_Wolf.h"
#include "Monster/BlackKnight/R1Monster_BlackKnight.h"
#include "Environment/Environment_Cloud.h"

#include "Store/R1Npc.h"
#include "Trap.h"
#include "Environment/Environment_Floor.h"
#include "Interactive_Buildings/PlayerSpawner/Building_PlayerSpawner.h"
#include "Interactive_Buildings/Telescope/Building_Telescope.h"

/* Setting*/
#include "Engine/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"

/*Building*/
#include "Interactive_Buildings/ReconTower/R1ReconTower.h"

#include "Math/RotationMatrix.h"
#include "Kismet/GameplayStatics.h"
#include "R1GameModeBase.h"
#include "Particles/ParticleSystem.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraManager.h"
#include "Camera/CameraActor.h"

/* Widget*/
#include "UI/InGame/Fight/SelectSkill_Widget.h"
#include "UI/InGame/PlayerBar_Widget.h"
#include "UI/InGame/PlayerHP_Widget.h"
#include "UI/InGame/Mission_Widget.h"
#include "UI/Lobby/PlayerSetting_Widget.h"
#include "UI/InGame/Second_Widget.h"
#include "UI/InGame/Fight/MonsterHP_Widget.h"
#include "UI/InGame/Fight/ResultFight_Widget.h"
#include "UI/InGame/Inventory/PlayerInventory_Widget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"

#include "Environment/Environment_Cliff.h"
#include "Engine/TriggerBox.h"

#define FAIILING_DEADLINE 40.f

AR1Player::AR1Player()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 마우스 방향으로 회전할 수 있도록 한다,
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f); //스르르 회전하도록 한다. (회전 보간)

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponnet"));
	m_SpringArm->SetupAttachment(GetCapsuleComponent());
	m_SpringArm->bDoCollisionTest = false;

	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Camera->SetupAttachment(m_SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 교차 오버랩
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AR1Player::OnBeginOverlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// 3D Widget
	CurrentState_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PLAYER_3DWIDGET"));
	CurrentState_Widget->SetupAttachment(GetMesh());
	CurrentState_Widget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	CurrentState_Widget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget> CurrentWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/InFight/WBP_FightCurrentState.WBP_FightCurrentState_C'"));
	if (CurrentWidget.Succeeded())
	{
		CurrentState_Widget->SetWidgetClass(CurrentWidget.Class);
		CurrentState_Widget->SetDrawSize(FVector2D(1000, 1000));
	}

	// Perticle : jump
	static ConstructorHelpers::FObjectFinder<UParticleSystem> JUMP_ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Clouds_02.Par_LoPo_Clouds_02'"));
	if (JUMP_ParticleSystem.Succeeded())
		Jump_Particle = JUMP_ParticleSystem.Object;
}

void AR1Player::BeginPlay()
{
	Super::BeginPlay();

	if (!Setting_PlayerState())
		MSG_BOX(TEXT("AR1Player() : 플레이어 상태가 제대로 세팅되지 않았습니다."));
	
	if (CurrentPlayerState)
	{
		// 이전 [Fight Scene] / 현재 [InGame Scene]
		if ((ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_PrevScene && ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene))
		{
			bLoadPlayer = true;
		}
	}

	// 1. 3D Widget Load 
	if (CurrentState_Widget)
	{
		CurrentState_Widget->InitWidget();
		WBPCurrent_Instance = CurrentState_Widget->GetUserWidgetObject();
	}

	Load_Assets();
	CameraSetting();

	InitPos = this->GetActorLocation();
	this->Set_HpText(MaxHp, Hp);
	this->Set_AttackText(TargetAttack);
	this->Set_HillText(TargetHill);

}

void AR1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Setting_Loading())
		MSG_BOX(TEXT("AR1Player() : 로드에 실패했습니다."));

	if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
		// 총기를 담당한 플레이어는 Moving할 수 없음
		if(CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)] || 
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM_ATTACK)])
		{
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)] = false;
		}

		if (true == bBossMap_PlayerChange)
		{
			Change_Player();
			bBossMap_PlayerChange = false;
		}
	}

	Falling_Reset();

	if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)])
		Player_Dead();
}

FVector AR1Player::Get_PlayerCameraCompPos()
{
	if (m_Camera)
		return m_Camera->GetComponentLocation();
	else
		return FVector{};
}

void AR1Player::Change_Level(ESCENE _scene)
{
	/* Game Mode의 Player 를 빼서 GameInstace로 값을 전달한다.*/
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		// 1. Save Player 
		for (int i = 0; i < LAST_PLAYER; i++)
		{
			if (GameInstance->Load_Players.IsValidIndex(i))
			{
				GameInstance->Load_Players[i].Player_CurrentScene = _scene;
				GameInstance->Load_Players[i].Player_PrevScene = GameMode->PlayerStates[i]->Player_CurrentScene;
				GameInstance->Load_Players[i]._JobType = GameMode->PlayerStates[i]->Player_Job;
				GameInstance->Load_Players[i].Current_DeadLine = GameMode->PlayerStates[i]->Current_DeadLine;
				GameInstance->Load_Players[i].Total_Coin = GameMode->PlayerStates[i]->TotalCoins;
				
				// Material Save
				for(int j = 0; j< sizeof(GameMode->PlayerStates[i]->Registration_Material) / sizeof(GameMode->PlayerStates[i]->Registration_Material[0]); j++)
					GameInstance->Load_Players[i].Registration_Material[j] = GameMode->PlayerStates[i]->Registration_Material[j];

				// 0. Save Last Position : InGame으로 넘어가는 게 아닐 때만 Last Position 저장
				if(ESCENE::INGAME != _scene)
				{
					GameInstance->Load_Players[i].LastPlayerPosition = GameMode->PlayerStates[i]->LastPlayerPosition;
				}
				else if (ESCENE::INGAME == _scene)
				{
					GameInstance->Load_Players[i].Target_MonsterType.Empty();

					// Fight Scene -> InGame Scene 인 경우 Player가 죽었을 때
					if (true == this->CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::DEAD)])
					{
						GameInstance->Load_Players[i].bDead = true;
					}
				}

				// 1. Mission
				TArray<UUserWidget*> FoundWidgets;
				UObject* WorldContextObject = GetWorld();
				FoundWidgets = AR1Player::FindWidgetsByClass<UMission_Widget>(WorldContextObject);
				if(FoundWidgets.Num() > 0)
					GameInstance->Load_Players[i].MissionType = Cast<UMission_Widget>(FoundWidgets[0])->Get_CurrentMission();

				// 2. Last Controller Player : 마지막에 컨트롤러를 쥐고 있던 Player 
				if (GameMode->CurrentPlayer == GameMode->PlayerArray[i])
				{
					if (GameMode->CurrentPlayer->Get_CurrentPlayerState()->FightTurnType_Array.Num() > 0)
					{
						for (auto iter : GameMode->CurrentPlayer->Get_CurrentPlayerState()->FightTurnType_Array)
						{
							GameInstance->Load_Players[i].Target_MonsterType.Emplace(iter->Get_MonsterType());
							GameInstance->DestroyMonster_Number.Emplace(iter->Get_MonsterNumber());
						}
					}

					GameInstance->Load_Players[i].Hill = GameMode->PlayerArray[i]->FinalHill;
					GameInstance->Load_Players[i].Hp = GameMode->PlayerArray[i]->Hp;
					GameInstance->Load_Players[i].MaxHp = GameMode->PlayerArray[i]->MaxHp;
					GameInstance->Load_Players[i].CurrentAttack = GameMode->PlayerArray[i]->CurrentAttack;

					GameInstance->Load_Players[i].bTarget_Player = true;
				}
				else
					GameInstance->Load_Players[i].bTarget_Player = false;
			}
		}
		
		// 현재 InGame Scene일 때만 저장
		if (ESCENE::INGAME != _scene)
		{
			// 0 플레이어 상태 
			for(int i = 0 ;i < LAST_PLAYER; i++)
			{
				GameInstance->Load_Players[i].Hill = GameMode->PlayerArray[i]->FinalHill;

				GameInstance->Load_Players[i].Hp = GameMode->PlayerArray[i]->Hp;
				GameInstance->Load_Players[i].MaxHp = GameMode->PlayerArray[i]->MaxHp;
				GameInstance->Load_Players[i].CurrentAttack = GameMode->PlayerArray[i]->CurrentAttack;
			}

			// 1. Chaos
			TArray<UUserWidget*> FoundWidgets;
			UObject* WorldContextObject = GetWorld();
			FoundWidgets = AR1Player::FindWidgetsByClass<UPlayerBar_Widget>(WorldContextObject);

			if (UPlayerBar_Widget* pPlayerBar = Cast<UPlayerBar_Widget>(FoundWidgets[0]))
			{
				GameInstance->Chaos_Information = pPlayerBar->Get_Chaos_Information();
				GameInstance->Turn_Cnt = pPlayerBar->Get_TurnCnt();
			}

			// 2. Save Npc 
			TArray<AR1Npc*> Npc_Array = AR1Player::FindActorsByClass<AR1Npc>(WorldContextObject);
			for (int32 Index = 0; Index < Npc_Array.Num(); ++Index)
			{
				if (Npc_Array[Index] == nullptr)
					continue;

				// 인덱스가 유효한지 확인
				if (Index < GameInstance->Load_Npcs.Num())
				{
					GameInstance->Load_Npcs[Index].NpcType = Npc_Array[Index]->Get_NpcType();
					GameInstance->Load_Npcs[Index].bEndDialog = Npc_Array[Index]->Get_NpcEndDialog();
					GameInstance->Load_Npcs[Index].bPrevLookAtRotation = Npc_Array[Index]->Get_PrevLookAtRotation();
					GameInstance->Load_Npcs[Index].NextCnt = Npc_Array[Index]->Get_DialogCnt();
				}
			}
		}

		// 3. Save Inventory
		if (CurrentPlayerState->ItemAssets.Num() > 0)
		{
			for (auto iter : CurrentPlayerState->ItemAssets)
				GameInstance->ItemAssets.Emplace(iter);
		}
	}

	if (ESCENE::INGAME == _scene)
		UGameplayStatics::OpenLevel(this, FName("Game1"));

	else if (ESCENE::FIGHT_SCENE == _scene)
		UGameplayStatics::OpenLevel(this, FName("Fight_Level"));

	else if (ESCENE::BOSS_SCENE == _scene)
		UGameplayStatics::OpenLevel(this, FName("BossTower"));
}

void AR1Player::Load_Assets()
{
	UWorld* World = GetWorld();
	if (World)
	{
		UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(World));
		if (GameInstance)
		{ 
			if (nullptr == CurrentPlayerState)
			{
				MSG_BOX(TEXT("R1Player : CurrentPlayerState()가 존재하지 않습니다."));
				return;
			}

			if (CurrentPlayerState)
			{
				if (CurrentPlayerState->Player_CurrentScene == ESCENE::LOBBY)
					Setting_LobbyScene();

				else if (CurrentPlayerState->Player_CurrentScene == ESCENE::INGAME)
					Setting_IngameScene();

				else if (CurrentPlayerState->Player_CurrentScene == ESCENE::FIGHT_SCENE)
					Setting_InFightScene();

				else if (CurrentPlayerState->Player_CurrentScene == ESCENE::BOSS_SCENE)
					Setting_IngameScene();
			}

			for (auto iter : GameInstance->ItemAssets)
				Add_InventoryItem(iter);

			// 크랙 뜸
			/*if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene &&
				ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_PrevScene)
				Set_ChaosMove_AndNextTurnStart();*/
		}
	}
}

/* Monster 로부터 받은 행동 변화*/
void AR1Player::Impact_MonsterPattern()
{
	// Monster의 영향
	if (!CurrentPlayerState || !CurrentPlayerState->TargetMonster)
		return;

	if (ESCENE::BOSS_SCENE != CurrentPlayerState->Player_CurrentScene)
	{
		if (true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] ||
			true == CurrentPlayerState->TargetMonster->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
		{
			if (EIMPACT::IMPACT_ATTACK != IsImpact)
			{
				IsImpact = EIMPACT::IMPACT_ATTACK;

				if (true == CalculateAbility((ABILITY_TYPE::DEFENCE)))
				{
					CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)] = true;
				}
				else
				{
					CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = true;
				}
			}
		}
		else
			IsImpact = EIMPACT::IMPACT_END;
	}
}

bool AR1Player::ShirkJump()
{
	if (false == bIsShirkJumping)
	{
		Super::Jump();

		FVector CurrentPos = this->GetActorLocation();
		FVector NewPos = FMath::VInterpTo(CurrentPos, Fight_ShirkPosition, GetWorld()->DeltaTimeSeconds, 5.f);
		this->SetActorLocation(NewPos);

		if (FVector::Dist(CurrentPos, Fight_ShirkPosition) < 50.f)
		{
			if (GetCharacterMovement()->IsMovingOnGround())
			{
				bIsShirkJumping = true;
				return false;
			}
		}
	}
	else if (true == bIsShirkJumping)
	{
		if (true == bShirkEnd)
		{
			Super::Jump();
			FVector CurrentPos = this->GetActorLocation();
			FVector NewPos = FMath::VInterpTo(CurrentPos, InitPos, GetWorld()->DeltaTimeSeconds, 5.f);
			this->SetActorLocation(NewPos);

			if (FVector::Dist(CurrentPos, InitPos) < 50.f)
			{
				if (GetCharacterMovement()->IsMovingOnGround())
				{	
					ShirkJumpStop();
					return false;
				}
			}
		}
	}
	return true;
}

void AR1Player::ShirkJumpStop()
{
	bIsShirkJumping = false;
	bShirkEnd = false;
}

void AR1Player::HitJump()
{
	if (!bIsJumping)
	{
		// Boss Map에서 점프 때문에 에러남
		Super::Jump();
		bIsJumping = true; 
	}
}

void AR1Player::HitJumpStop()
{
	Super::StopJumping();
	bIsJumping = false;
}

// 플레이어를 뒤로 밀기 시작한다.
void AR1Player::Start_PullBack()
{
	if (false == bPullBack_End)
		bPullBack_Start = true;
}

// Player를 뒤로 민다
bool AR1Player::PullBack()
{
	if (true == bPullBack_End)
	{
		if (Return_InitPos())
		{
			bPullBack_Start = false;
			bPullBack = false;
		}
		return true;
	}

	if(false == bPullBack)
	{
		PullBackPos = InitPos;
		PullBackPos.X += 200.f;
		bPullBack = true;
	}

	// 뒤로 밀리기
	FVector NewPos = FMath::VInterpTo(this->GetActorLocation(), PullBackPos, GetWorld()->GetDeltaSeconds(), 4.f);
	this->SetActorLocation(NewPos);

	if (10.f >= FVector::Dist(NewPos, PullBackPos))
	{
		PullBack_Timer += GetWorld()->GetDeltaSeconds();

		if(1.f <= PullBack_Timer)
		{
			PullBack_Timer = 0.f;
			bPullBack_End = true;
		}

		return true;
	}
	return false;
}

bool AR1Player::Return_InitPos()
{
	FVector NewPos = FMath::VInterpTo(this->GetActorLocation(), InitPos, GetWorld()->GetDeltaSeconds(), 2.f);
	this->SetActorLocation(NewPos);

	if (10.f >= FVector::Dist(NewPos, InitPos))
		return true;
	return false;
}

void AR1Player::OnTrapStep()
{
	if (bIsJumping)
	{
		AR1PlayerController* PC = Cast<AR1PlayerController>(GetController());
		PC->BackRotation_Jump();
	}
}


void AR1Player::Setting_LobbyScene()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		if (this == GameMode->CurrentPlayer)
		{
			PlayerSetting_Instance = Cast<UPlayerSetting_Widget>(CreateWidget(GetWorld(), PlayerSetting_Widget));
			if (PlayerSetting_Instance)
				PlayerSetting_Instance->AddToViewport();
			else
				MSG_BOX(TEXT("Failed : PlayerSetting_Instance"));
		}
	}
}

bool AR1Player::Get_ShowClassInfo()
{
	if (!PlayerSetting_Instance)
		return false;

	if (ESlateVisibility::Hidden == PlayerSetting_Instance->Get_ShowWidget())
		return true;
	else
		return false;
}

void AR1Player::Set_SelectTargetMonster(AR1Monster* _monster)
{
	CurrentPlayerState->TargetMonster = _monster;
}

void AR1Player::Setting_IngameScene()
{
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		for (int i = 0; i < LAST_PLAYER; i++)
		{
			if (this == GameMode->PlayerArray[i])
			{
				/* Player HP Bar*/
				PlayerHP_Instance = Cast<UPlayerHP_Widget>(CreateWidget(GetWorld(), PlayerHP_Widget));
				if (PlayerHP_Instance)
				{
					PlayerHP_Instance->AddToViewport();
					PlayerHP_Instance->Set_Player(GameMode->PlayerArray[i]);
					PlayerHP_Instance->Set_CanvasPos(i);
				}
				else
					MSG_BOX(TEXT("Failed : PlayerHP_Instance"));

				break;
			}
		}

		/* Inventory */
		Inventory_Instance = Cast<UPlayerInventory_Widget>(CreateWidget(GetWorld(), Inventory_Widget));
		if (Inventory_Instance)
		{
			Inventory_Instance->AddToViewport(5);
			Inventory_Instance->Set_OwnerPlayer(this);
		}
		else
			MSG_BOX(TEXT("Failed : Inventory_Instance"));

		if (this == GameMode->PlayerArray[0])
		{
			/* Result */
			ResultFight_Instance = Cast<UResultFight_Widget>(CreateWidget(GetWorld(), ResultFight_Widget));
			if (ResultFight_Instance)
				ResultFight_Instance->AddToViewport(3);
			else
				MSG_BOX(TEXT("Failed : Result Fight Widget"));

			/* Mission Widget*/
			Mission_Instance = Cast<UMission_Widget>(CreateWidget(GetWorld(), Mission_Widget));
			if (Mission_Instance)
			{
				Mission_Instance->AddToViewport();
				if (true == bLoadPlayer)
					Mission_Instance->Set_CurrentMission(GameInstance->Load_Players[0].MissionType);
			}
			else
				MSG_BOX(TEXT("Failed : Mission_Instance"));

			if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
			{
				/* Second Widget*/
				Second_Instance = Cast<USecond_Widget>(CreateWidget(GetWorld(), Second_Widget));
				if (Second_Instance)
					Second_Instance->AddToViewport();
				else
					MSG_BOX(TEXT("Failed : Second_Instance"));
			}

			/* Player Bar */
			PlayerBar_Instance = Cast<UPlayerBar_Widget>(CreateWidget(GetWorld(), PlayerBar_Widget));
			if (PlayerBar_Instance)
			{
				PlayerBar_Instance->AddToViewport();

				if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
				{
					PlayerBar_Instance->Set_FightType(EFIGHT_AUTHORITY::MONSTER);
				}
			}
			else
				MSG_BOX(TEXT("Failed : PlayerBar_Instance"));
		}
	}
}

void AR1Player::Setting_InFightScene()
{
	// 1. 몬스터 생성
	if (!Create_FightMonster())
		MSG_BOX(TEXT("R1Player() : Failed Create Monster"));

	/* 2. Widget 생성*/
	/* Result Fight Widget 생성 */
	ResultFight_Instance = Cast<UResultFight_Widget>(CreateWidget(GetWorld(), ResultFight_Widget));
	if (ResultFight_Instance)
		ResultFight_Instance->AddToViewport(3);
	else
		MSG_BOX(TEXT("Failed : Result Fight Widget"));

	/* Player Bar */
	PlayerBar_Instance = Cast<UPlayerBar_Widget>(CreateWidget(GetWorld(), PlayerBar_Widget));
	if (PlayerBar_Instance)
		PlayerBar_Instance->AddToViewport();
	else
		MSG_BOX(TEXT("Failed : PlayerBar Widget"));

	/* Skill Widget 생성 */
	SkillWidget_Instance = Cast<USelectSkill_Widget>(CreateWidget(GetWorld(), Skill_Widget));
	if (SkillWidget_Instance)
		SkillWidget_Instance->AddToViewport();
	else
		MSG_BOX(TEXT("Failed : Skill Widget"));

	/* Player HP Bar*/
	PlayerHP_Instance = Cast<UPlayerHP_Widget>(CreateWidget(GetWorld(), PlayerHP_Widget));
	if (PlayerHP_Instance)
		PlayerHP_Instance->AddToViewport();
	else
		MSG_BOX(TEXT("Failed : PlayerHP Widget"));

	// 3. 플레이어의 지정 위치와 회피 위치
	InitPos = this->GetActorLocation();
	Fight_ShirkPosition = InitPos;
	Fight_ShirkPosition.X += 200.f;
	Fight_ShirkPosition.Y += 200.f;
}

bool AR1Player::Setting_PlayerState()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameMode)
	{
		// 1. Fight Scene일 경우
		if (ESCENE::FIGHT_SCENE == GameMode->PlayerStates[0]->Player_CurrentScene)
		{
			int32 PlayerCnt = { 0 };
			for (auto iter : GameInstance->Load_Players)
			{
				if (true == iter.bTarget_Player)
					break;
				else
					++PlayerCnt;
			}

			GameMode->PlayerArray[PlayerCnt] = this;
			GameMode->CurrentPlayer = this;
			CurrentPlayerState = GameMode->PlayerStates[PlayerCnt];

			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->Possess(this);
				AR1PlayerController* R1PlayerController = Cast<AR1PlayerController>(PlayerController);
				if (R1PlayerController)
				{
					R1PlayerController->Set_Highlight_Character(this);
					Highlight();
				}
				else
				{
					MSG_BOX(TEXT("R1PlayerController 변환 형식이 잘못되었습니다."));
				}
			}

			if(nullptr == CurrentPlayerState)
				return false;
		}
		else
		{
			for (int i = 0; i < LAST_PLAYER; i++)
			{
				if (nullptr == GameMode->PlayerArray[i])
				{
					GameMode->PlayerArray[i] = this;

					if (nullptr == CurrentPlayerState)
					{
						// 상태 값 담아주기
						if (GameMode->PlayerStates[i])
						{
							CurrentPlayerState = GameMode->PlayerStates[i];
						}
						else
							return false;
					}

					// 1. (로비 -> 인게임 || 인게임 ) 인 경우 || [BoassMap]: 가장 첫 번째 플레이어가 주도권을 가져간다.
					if ((ESCENE::LOBBY == CurrentPlayerState->Player_PrevScene && ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene ||
						ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene) || 
						ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
					{
						// 1. 먼저 주도권을 잡을 Player
						if (0 == i)
						{
							APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
							if (PlayerController)
								PlayerController->Possess(this);

							AR1PlayerController* R1PlayerController = Cast<AR1PlayerController>(PlayerController);
							if (R1PlayerController)
							{
								R1PlayerController->Set_Highlight_Character(this);
								Highlight();
							}
							else
							{
								MSG_BOX(TEXT("R1PlayerController 변환 형식이 잘못되었습니다."));
							}

							GameMode->CurrentPlayer = this;
						}
					}
					// 2. (파이트 -> 인게임) 인 경우 : 마지막에 싸웠던 플레이어가 주도권을 가져간다.
					else
					{
						if (true == CurrentPlayerState->bNextTarget)
						{
							APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
							if (PlayerController)
								PlayerController->Possess(this);

							AR1PlayerController* R1PlayerController = Cast<AR1PlayerController>(PlayerController);
							if (R1PlayerController)
							{
								R1PlayerController->Set_Highlight_Character(this);
								Highlight();
							}
							else
							{
								MSG_BOX(TEXT("R1PlayerController 변환 형식이 잘못되었습니다."));
							}
							GameMode->CurrentPlayer = this;
						}
					}

					// 3. 불구하고 Controller Player를 찾지 못했을 때 
					if (i == 2)
					{
						if (nullptr == GameMode->CurrentPlayer)
						{
							APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
							if (PlayerController)
								PlayerController->Possess(this);

							AR1PlayerController* R1PlayerController = Cast<AR1PlayerController>(PlayerController);
							if (R1PlayerController)
							{
								R1PlayerController->Set_Highlight_Character(this);
								Highlight();
							}
							else
							{
								MSG_BOX(TEXT("R1PlayerController 변환 형식이 잘못되었습니다."));
							}

							GameMode->CurrentPlayer = this;
						}
					}
					break;
				}
			}
		}
	}
	return true;
}

bool AR1Player::Setting_Loading()
{
	// 기억 안남 : 뭐지
	if (!WBPCurrent_Instance)
	{
		if (CurrentState_Widget)
		{
			CurrentState_Widget->InitWidget();
			WBPCurrent_Instance = Cast<UUserWidget>(CurrentState_Widget->GetUserWidgetObject());
		}
	}

	// 기본 로딩 정보
	if (!Setting_TickLoading())
		return false;

	// Fight -> InGame
	if (bLoadPlayer)
	{
		if (PlayerBar_Instance)
			PlayerBar_Instance->Load_Chaos_Information();

		if (!Load_Completed())
			return false;
	}
	return true;
}

bool AR1Player::Setting_TickLoading()
{
	// 1. 개별적인 부분 로딩
	if (false == bIndependentLoading)
	{
		bIndependentLoading = true;

		// Load : Player State 
		this->Set_HpText(MaxHp, Hp);
		this->Set_AttackText(TargetAttack);
		this->Set_HillText(TargetHill);
		this->Update_MaxHp(0, true);

		// Load : Material
		for (int i = 0; i < sizeof(CurrentPlayerState->Registration_Material) / sizeof(CurrentPlayerState->Registration_Material[0]); i++)
		{
			if (nullptr != CurrentPlayerState->Registration_Material[i])
				this->Load_PlayerMaterial(static_cast<CUSTOMIZE_PLAYER>(i), CurrentPlayerState->Registration_Material[i]);
		}
	}
	return true;
}

void AR1Player::OnBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}


void AR1Player::CameraSetting()
{
	if(CurrentPlayerState)
	{
		switch (CurrentPlayerState->Player_CurrentScene)
		{
		case ESCENE::LOBBY:
			//GetCharacterMovement()->bOrientRotationToMovement = true;
			//m_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f)/* Y,Z,X 순서*/);
			//m_SpringArm->TargetArmLength = 300.f;
			break;

		case ESCENE::INGAME:

			GetCharacterMovement()->bOrientRotationToMovement = true;

			m_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(-60.f, 0.f, 0.f)/* Y,Z,X 순서*/);
			m_SpringArm->TargetArmLength = 1200.f; 
			break;

		case ESCENE::FIGHT_SCENE:
			GetCharacterMovement()->bOrientRotationToMovement = false;
			break;

		case ESCENE::BOSS_SCENE:
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;

			m_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(-60.f, 0.f, 0.f)/* Y,Z,X 순서*/);
			m_SpringArm->TargetArmLength = 1200.f;

			SceneCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), GetActorLocation() + FVector(14.f, 21080.f, 875.f), FRotator(0, -90.f, 0));
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController && SceneCamera)
			{
				PlayerController->SetViewTarget(SceneCamera);
			}
		}
		break;

		default:
			break;
		}
	}
}

void AR1Player::Player_Dead()
{
	/*if (Hp <= 0.f)
		MSG_BOX(TEXT("플레이어 주금"));*/
	
	UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance->DestroyMonster_Number.Num() > 0)
		GameInstance->DestroyMonster_Number.Pop();

	this->Destroy();
}

/* 그냥 때려야 할 때*/
bool AR1Player::Attack()
{
	if (false == bAttackStart)
	{
		bAttackStart = true;
		return true;
	}
	else
		return false;
}

/* 때리는 타이밍에 맞춰서 때려야 할 때 : Animation에서 전달해 줄 것임*/
bool AR1Player::TimingAttack()
{
	return bAttackStart;
}

void AR1Player::Stop_TimingAttack()
{
	bAttackStart = false;
}

bool AR1Player::StopAttack(EPLAYER_PATTERN _ePattern)
{
	if(Ready_NextTurn(_ePattern, EFIGHT_AUTHORITY::MONSTER))
	{
		bAttackStart = false;
		return true;
	}
	else
		return false;
}

void AR1Player::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bIsJumping)
	{
		HitJumpStop();
	}
}

// 맞을 때 사용
bool AR1Player::Hit()
{
	if(false == HPAmount)
	{
		HPAmount = true;
		if(nullptr != CurrentPlayerState->TargetTrap)
		{	
			OnDamged(CurrentPlayerState->TargetTrap->FinalDamage, nullptr);
			CurrentPlayerState->TargetTrap = nullptr;
			HitJump();
			Drop_Coin(this->GetActorLocation());
		}
		else
		{
			OnDamged(CurrentPlayerState->TargetMonster->CurrentAttack, CurrentPlayerState->TargetMonster);
		}
	}
	OnTrapStep();
	DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());
	if (CurrentHp == Hp)
	{
		HPAmount = false;
		return true;
	}
	else
		return false;
}

// 힐 사용
bool AR1Player::Hill()
{
	if (false == HPAmount)
	{
		HPAmount = true;
		OnHill(FinalHill, this);
	}
	DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());

	if (CurrentHp == Hp)
	{
		HPAmount = false;
		return true;
	}
	else
		return false;
}

bool AR1Player::HitAndDash(AR1Character* _thisObj, FVector _targetPos, bool _endTiming)
{
	bHitAndDashStart = true;
	if (false == bHitAndDash)
		bHitAndDash = Super::HitAndDash(_thisObj, _targetPos, _endTiming);
	else
		HitAndDash_Timer = 0.f;

	if (true == bCompletedDash)
	{
		HitAndDash_Timer += GetWorld()->GetDeltaSeconds();

		/* 몬스터가 물러날 때 살짝의 텀을 주고 회피하고 싶음*/
		if (HitAndDash_Timer >= 0.25f)
		{
		}
	}
	return bHitAndDash;
}

void AR1Player::Stop_HitAndDash()
{
	bHitAndDashStart = false;
	bHitAndDash = false;
}

// 프로그래스 바 자연스럽게 조절
void AR1Player::DecreaseHP(float& _currentHP, float _targetHP, float DeltaTime, float _speed)
{
	Super::DecreaseHP(_currentHP, _targetHP, DeltaTime, _speed);
	if(PlayerHP_Instance)
	{
		float progressBarValue = CurrentHp / MaxHp;
		PlayerHP_Instance->Set_HpProgressBar(FMath::Clamp(progressBarValue, 0.f, 1.f)); // 프로그래스 바
		this->Set_HpText(MaxHp, CurrentHp); // 수치
	}
}

/* 해제시킬 패턴과, 다음 턴의 인물*/
bool AR1Player::Ready_NextTurn(EPLAYER_PATTERN _pattern, EFIGHT_AUTHORITY _nextTarget)
{
	NextTurn_ReadyTimer += GetWorld()->GetDeltaSeconds();
	if (NextTurn_ReadyTimer >= 1.5f)
	{
		NextTurn_ReadyTimer = 0.f;

		bNextTurn = false;

		CurrentPlayerState->Player_Pattern[static_cast<int32>(_pattern)] = false;

		if(false == bRidAllMonster)
			Set_FightAuthority(_nextTarget); // 캐릭터 썸네일 변경

		Next_Energy();
		return true;
	}
	return false;
}

void AR1Player::Next_Energy()
{
	if (AR1Monster* NextMonster = Cast<AR1Monster>(PlayerBar_Instance->Get_NextCharacterType_Array()[1]))
		NextMonster->Set_EnergyStart(true);
}

bool AR1Player::Get_GamePlayStart()
{
	ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);

	if (nullptr == MyCameraManager)
		return false;

	return MyCameraManager->Get_GamePlayStart();
}

bool AR1Player::Change_Player()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	int Cnt = { 0 };
	for (int i = 0; i < LAST_PLAYER; i++)
	{
		if (GameMode->CurrentPlayer == GameMode->PlayerArray[i])
		{
			if (sizeof(GameMode->PlayerArray) / sizeof(GameMode->PlayerArray[0]) <= i + 1)
				GameMode->CurrentPlayer = GameMode->PlayerArray[0];
			else
				GameMode->CurrentPlayer = GameMode->PlayerArray[i + 1];

			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				PlayerController->Possess(GameMode->CurrentPlayer);

				// 카메라 조정
				ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
				MyCameraManager->Set_PrevCamPos(MyCameraManager->Get_CameraActor()->GetComponentLocation());
				AActor* ViewTarget = PlayerController->GetViewTarget();
				MyCameraManager->Set_CameraActor(ViewTarget->FindComponentByClass<UCameraComponent>());
				MyCameraManager->Get_CameraActor()->SetWorldLocation(MyCameraManager->Get_PrevCamPos());

				return true;
			}
		}
	}
	return false;
}

bool AR1Player::Get_MouseMoving()
{
	if (Second_Instance)
		return Second_Instance->Get_MouseMoving();
	else
		return false;
}

void AR1Player::Set_StopMouseMoving()
{
	if (Second_Instance)
		return Second_Instance->Set_StopMouseMoving();
}

void AR1Player::Add_InventoryItem(FItemData_Asset* _asset)
{
	if (Inventory_Instance)
		Inventory_Instance->AddWidgetToVerticalBox(_asset);
}

bool AR1Player::Falling_Player()
{
	if (true == bFalling)
		return true;

	if (FAIILING_DEADLINE >= GetActorLocation().Z)
	{
		TArray<UUserWidget*> MyWidgets;
		TArray<UUserWidget*> MyWidgets_Second;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), MyWidgets, UResultFight_Widget::StaticClass(), false);
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), MyWidgets_Second, USecond_Widget::StaticClass(), false);

		if (MyWidgets.Num() > 0)
		{
			UResultFight_Widget* ResultWidget = Cast<UResultFight_Widget>(MyWidgets[0]);
			USecond_Widget* SecondWidget = Cast<USecond_Widget>(MyWidgets_Second[0]);

			if (ResultWidget && SecondWidget)
			{
				SecondWidget->Second_Configure();
				ResultWidget->Set_CallResultWidget(true);
				++CurrentPlayerState->Current_DeadLine;
				bFalling = true;
				return true;
			}
		}
	}
	return false;
}

void AR1Player::Falling_Reset()
{
	if (true == Falling_Player())
	{
		// 플레이어를 원래 자리로 둔다.
		FallingTimer += GetWorld()->GetDeltaSeconds();
		if (0.5f <= FallingTimer)
		{
			// 2. Spawn on Specified Spawner
			AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
			// 2.1 Find Player Cnt
			int32 iPlayerCnt = { 0 };
			for (auto iter : GameMode->PlayerArray)
			{
				if (this == iter)
					break;
				else
					++iPlayerCnt;
			}

			// 2.2 Find Player Specified Spawner (? 가장 가까운?)
			UObject* WorldContextObject = GetWorld();
			TArray<ABuilding_PlayerSpawner*> SpawnerArray = AR1Player::FindActorsByClass<ABuilding_PlayerSpawner>(WorldContextObject);

			double		PrevMinDistResult = {};
			FVector		NearestSpanwer = {};
			int32			iFindCnt = { 0 };
			for (auto iter : SpawnerArray)
			{
				if (ESPAWN_TYPE::BLUE_SPAWN == iter->Get_SpawnType())
				{
					NearestSpanwer = iter->GetActorLocation();
					break;
				}
			}

			// 가장 가까운 스포너를 찾아 스폰시킨다.
			NearestSpanwer.Z = NearestSpanwer.Z + 200.f;
			this->SetActorLocation(NearestSpanwer);

			if (true == End_Turn(true))
			{
				FallingTimer = 0.f;
				bFalling = false;
			}
		}
	}
}

void AR1Player::Set_NextTurnStart(bool _start)
{
	if(Second_Instance)
		Second_Instance->Set_SecondWidget_Start(_start);
}

// 카오스도 함께 이동
void AR1Player::Set_ChaosMove_AndNextTurnStart(bool _start)
{
	if(PlayerBar_Instance)
		PlayerBar_Instance->MoveChaosAndNextTurn(_start);
}

// 턴을 종료할 때 줌 Scene이 필요한가 초기화가 필요한가?
bool AR1Player::End_Turn(bool _bneedzoom)
{
	TArray<UUserWidget*> MyWidgets;
	TSubclassOf<UUserWidget> MyWidgetClass = USecond_Widget::StaticClass();

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), MyWidgets, MyWidgetClass, false);
	if (MyWidgets.Num() > 0)
	{
		USecond_Widget* SecondInst = Cast<USecond_Widget>(MyWidgets[0]);
		if (SecondInst)
		{
			if (false == _bneedzoom)
				return SecondInst->Second_Configure();

			else if (true == _bneedzoom)
				return SecondInst->TurnEndZoomTransition();
		}
	}
	return false;
}

// 상호작용이 가능한가?
bool AR1Player::IsInteractable()
{
	ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);

	if(!Second_Instance)
		return (MyCameraManager->Get_Switching_Camera()) ? false : true;
	else
		return (Second_Instance->Get_SecondWidget_Start() && MyCameraManager->Get_Switching_Camera()) ? false : true;
	// 초를 세는 동안이면서 카메라 매니저 스위칭이 돌아가는 중이라면 false
}

bool AR1Player::Update_MaxHp(int _value, bool _directly)
{
	if (false == bSaveTargetHp)
	{
		TargetMaxHp += _value;
		bSaveTargetHp = true;
		bSaveMaxPlus = (MaxHp <= TargetMaxHp); 
	}

	if(false == _directly)
	{
		// 1. Target Max 까지 보간하며 내려가기
		// 한 틱당 한 번 보간하기 때문에 int 형을 보간하려면 Lerp로 접근 or Stack으로 쌓아서 넘긴다.
		MaxHp += FMath::FInterpTo(fDWOrigin, TargetMaxHp, GetWorld()->GetDeltaSeconds(), 1.f);
		this->Set_HpText(MaxHp, Hp);

		// 3. MaxHp와 대비한 Hp의 ProgressBar를 조정해준다.
		if (true == bSaveMaxPlus)
			this->DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());
	}
	else if (true == _directly)
	{
		this->Set_HpText(MaxHp, Hp);
		if(PlayerHP_Instance)
		{
			// Max
			float progressBarValue = CurrentHp / MaxHp;
			PlayerHP_Instance->Set_HpProgressBar(FMath::Clamp(progressBarValue, 0.f, 1.f)); // 프로그래스 바
		}
	}

	// 2. Max == TargetMax 가 동일해졌는가?
	if ((bSaveMaxPlus && MaxHp >= TargetMaxHp) ||
		(!bSaveMaxPlus && MaxHp <= TargetMaxHp))
	{
		MaxHp = TargetMaxHp; 
		this->Set_HpText(MaxHp, Hp);

		bSaveTargetHp = false;
		fDWOrigin = 0.f;
		return true;
	}
	return false;
}

bool AR1Player::Update_MaxAttack(int _value)
{
	if (false == bSaveTargetAttack)
	{
		TargetAttack += _value;
		bSaveTargetAttack = true;
	}

	CurrentAttack += FMath::FInterpTo(fDWOrigin, TargetAttack, GetWorld()->GetDeltaSeconds(), 1.f);

	this->Set_AttackText(CurrentAttack);

	if (FMath::Abs(CurrentAttack - TargetAttack) < 0.01f) 
	{
		fDWOrigin = 0.f;
		CurrentAttack = TargetAttack;
		return true;
	}
	return false;
}

bool AR1Player::Update_MaxHill(int _value)
{
	if (false == bSaveTargetHill)
	{
		TargetHill += _value;
		bSaveTargetHill = true;
	}

	FinalHill += FMath::FInterpTo(fDWOrigin, TargetHill, GetWorld()->GetDeltaSeconds(), 1.f);

	this->Set_HillText(FinalHill);

	if (FMath::Abs(TargetHill - FinalHill) < 0.01f)
	{
		fDWOrigin = 0.f;
		FinalHill = TargetHill;
		return true;
	}
	return false;
}

void AR1Player::Set_HpText(float _maxHp, float _hp)
{
	if (PlayerHP_Instance)
		PlayerHP_Instance->Set_HpText(_maxHp, _hp);
}

void AR1Player::Set_AttackText(float _attack)
{
	if (PlayerHP_Instance)
		PlayerHP_Instance->Set_AttackText(_attack);
}

void AR1Player::Set_HillText(float _hill)
{
	if (PlayerHP_Instance)
		PlayerHP_Instance->Set_HillText(_hill);
}

void AR1Player::Set_FightAuthority(EFIGHT_AUTHORITY _type)
{
	if (true == bFightEnd)
		return;

	if (nullptr == PlayerBar_Instance)
		return;

	if (ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
		PlayerBar_Instance->Set_FightType(_type);
	}
	else
	{
		if (PlayerBar_Instance)
			PlayerBar_Instance->Set_NextTurnType();
	}
}

EFIGHT_AUTHORITY AR1Player::Get_FightAuthority()
{
	if (PlayerBar_Instance)
		return PlayerBar_Instance->Get_FightType();
	else
		return EFIGHT_AUTHORITY::END;
}

//void AR1Player::Set_MoveCursorUI(bool isTrue, UUserWidget* _myUserWidget)
//{
//	AR1PlayerController* PC = Cast<AR1PlayerController>(GetController());
//
//	if (PC)
//	{
//		if (isTrue)
//		{
//			/*	FInputModeUIOnly InputMode;s
//				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//				PC->SetInputMode(InputMode);
//
//				if (_myUserWidget)
//				{
//					_myUserWidget->SetUserFocus(PC);
//					PC->GetSlateOperations().SetUserFocus(_myUserWidget.toToSharedRef());
//				}*/
//
//		}
//		else
//		{
//			/*	MSG_BOX(TEXT("게임 모드로 전환합니다."));
//
//				FInputModeGameOnly InputMode;
//				PC->SetInputMode(InputMode);*/
//		}
//	}
//}

void AR1Player::Spawn_JumpParticle()
{
	if(Jump_Particle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Jump_Particle, this->GetActorLocation(), this->GetActorRotation());
}

bool AR1Player::Using_Item(EITEM_TYPE _item)
{
	switch (_item)
	{
	case EITEM_TYPE::APPLE : /* HP + 10 */
	{
		if (false == bUsingItem)
		{
			bUsingItem = true;
			Hp -= 10;
		}

		DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());
		bUsingItem = (FMath::Abs(CurrentHp - Hp) > 5.0f);
		if(false == bUsingItem)
			return true;
	}
	break;

	case EITEM_TYPE::POTION1 : /* HP + 30 */
	{
		if (Update_MaxHp(30))
			return true;
	}
	break;
	}
	return false;
}

// 에너지 관련
void AR1Player::Full_Energy(bool _full)
{
	if(PlayerHP_Instance)
		PlayerHP_Instance->Full_Energy(_full);
}

void AR1Player::Use_SkillEnergy(bool _use)
{
	if(SkillWidget_Instance)
		SkillWidget_Instance->Use_Energy(_use);
}

void AR1Player::Call_StateWidget(FText _text)
{
	if (!WBPCurrent_Instance)
		return;

	// 1. Widget Moving
	StateWidget_Timer += GetWorld()->GetDeltaSeconds();
	if (StateWidget_Timer <= 2.f)
	{
		/*UTextBlock* TB = {};
		for (auto iter : WBPCurrent_Instance->Get_Canvas()->GetAllChildren())
		{
			TB = Cast<UTextBlock>(iter);
			if (TB)
			{
				TB->SetText(_text);
				break;
			}
		}*/

		/*FVector2D CurrentWidgetPos = WBPCurrent_Instance->GetTextPosition(TB);
		CurrentWidgetPos.Y += GetWorld()->GetDeltaSeconds();
		WBPCurrent_Instance->MoveText(TB, CurrentWidgetPos);*/
	}
	else
	{
		/*WBPCurrent_Instance->Set_ShowWidget(false);
		StateWidget_Timer = 0.f;
		
		for (auto iter : WBPCurrent_Instance->Get_Canvas()->GetAllChildren())
		{
			UTextBlock* TB =  Cast<UTextBlock>(iter);
			if (TB)
			{
				WBPCurrent_Instance->MoveText(TB, Origin_StateWidget);
				break;
			}
		}*/
	}

	// 2. Look Cam Rotation
	if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
	{
		FVector Current_CamPos = MyCameraManager->GetCameraLocation();
		FVector Current_MeshPos = CurrentState_Widget->GetComponentLocation();

		FVector DirectionToCamera = (Current_CamPos - Current_MeshPos).GetSafeNormal();
		FRotator RotationToCam = DirectionToCamera.Rotation();
		CurrentState_Widget->SetWorldRotation(RotationToCam);
	}
}

// 플레이어가 에너지를 사용하는가?
bool AR1Player::Use_PlayerEnergy(bool _using)
{
	if (nullptr != CurrentPlayerState->TargetMonster)
	{
		// 1. InGame : Monster에게 어떻게 접근할 것인가?
		if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
		{
			if (PlayerHP_Instance->Use_Energy())
				CurrentPlayerState->TargetMonster->Get_UseEnergy(_using);
		}

		// 2. InFight : Monster와 어떻게 싸울 것인가?
		if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
		{
			if (PlayerHP_Instance->Use_Energy())
				Use_SkillEnergy(_using);
		}
		return true;
	}
	return false;
}

bool AR1Player::Add_PlayerEnergy(int32 _add)
{
	if (PlayerHP_Instance)
	{
		PlayerHP_Instance->Add_Energy(_add);
		return true;
	}
	return false;
}

void AR1Player::Player_MaterialCustomize(int32 _materialCnt, _int32 _customCnt, UPlayerSetting_Widget* _playerbarWbp)
{
	USkeletalMeshComponent* MeshCom = this->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshCom)
		return;

	if (CUSTOMIZE_PLAYER::HAIR_CUSTOM == PlayerMaterial_Cnt[_customCnt])
	{
		if (MeshCom && _playerbarWbp->Get_HairMaterial().Num() > 0 && (_materialCnt >= 0 && _materialCnt < _playerbarWbp->Get_HairMaterial().Num()))
		{
			MeshCom->SetMaterial(static_cast<int32>(_customCnt), _playerbarWbp->Get_HairMaterial()[_materialCnt]);
			CurrentPlayerState->Registration_Material[static_cast<int32>(_customCnt)] = _playerbarWbp->Get_HairMaterial()[_materialCnt];
		}
	}

	else if (CUSTOMIZE_PLAYER::FACE_CUSTOM == PlayerMaterial_Cnt[_customCnt])
	{
		if (MeshCom && _playerbarWbp->Get_SkinMaterial().Num() > 0 && (_materialCnt >= 0 && _materialCnt < _playerbarWbp->Get_SkinMaterial().Num()))
		{
			MeshCom->SetMaterial(static_cast<int32>(_customCnt), _playerbarWbp->Get_SkinMaterial()[_materialCnt]);
			CurrentPlayerState->Registration_Material[static_cast<int32>(_customCnt)] = _playerbarWbp->Get_SkinMaterial()[_materialCnt];
		}
	}

	else if (CUSTOMIZE_PLAYER::TOB_CUSTOM == PlayerMaterial_Cnt[_customCnt])
	{
		/*		FString CustomCntStr = FString::Printf(TEXT("%d"), _materialCnt);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, CustomCntStr);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *CustomCntStr);*/
		if (MeshCom && _playerbarWbp->Get_TopMaterial().Num() > 0 && (_materialCnt >= 0 && _materialCnt < _playerbarWbp->Get_TopMaterial().Num()))
		{
			MeshCom->SetMaterial(static_cast<int32>(_customCnt), _playerbarWbp->Get_TopMaterial()[_materialCnt]);
			CurrentPlayerState->Registration_Material[static_cast<int32>(_customCnt)] = _playerbarWbp->Get_TopMaterial()[_materialCnt];
		}
	}
}

void AR1Player::Load_PlayerMaterial(CUSTOMIZE_PLAYER _customType, UMaterialInterface* _material)
{	
	USkeletalMeshComponent* MeshCom = this->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshCom)
		return;

	MeshCom->SetMaterial(static_cast<int32>(_customType), _material);
}	

bool AR1Player::Create_FightMonster()
{
	/* 1. Create Monster */
	FVector SpawnLocation(0.f, 0.f, 400.f);
	FRotator SpawnRotation(0.f, 0.f, 0.f);

	if (0 > CurrentPlayerState->Target_MonsterType.Num())
	{
		MSG_BOX(TEXT("몬스터 생성 불가"));
		return false;
	}

	int32 FirstMonster = { 0 };
	for (auto iter : CurrentPlayerState->Target_MonsterType)
	{
		if (EMONSTER_TYPE::WIZARD == iter)
		{
			UClass* MonsterBPClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/Blueprints/Monster/BP_R1Wizard_Monster.BP_R1Wizard_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Wizard* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Wizard>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::BEE == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Bee::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Bee_Monster.BP_R1Bee_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Bee* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Bee>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}
		else if (EMONSTER_TYPE::CHEST == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Chest::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Chest_Monster.BP_R1Chest_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Chest* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Chest>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}
		else if (EMONSTER_TYPE::MUSHROOM == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Mushroom::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Mushroom_Monster.BP_R1Mushroom_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Mushroom* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Mushroom>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}
		else if (EMONSTER_TYPE::TURTLESHELL == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_TurtleShell::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1TurtleShell_Monster.BP_R1TurtleShell_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_TurtleShell* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_TurtleShell>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::SWARM == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Swarm::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Swarm_Monster.BP_R1Swarm_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Swarm* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Swarm>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::CRAP == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Crap::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Crap_Monster.BP_R1Crap_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Crap* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Crap>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::LIZARD == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_LizardWarrior::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1LizardWarrior_Monster1.BP_R1LizardWarrior_Monster1_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_LizardWarrior* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_LizardWarrior>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::RAT == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Rat::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Rat_Monster.BP_R1Rat_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Rat* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Rat>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::SPECTER == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Specter_Monster::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Specter_Monster.BP_R1Specter_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Specter_Monster* MonsterInstance = GetWorld()->SpawnActor<AR1Specter_Monster>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::WOLF == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_Wolf::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1Wolf_Monster.BP_R1Wolf_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_Wolf* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_Wolf>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}

		else if (EMONSTER_TYPE::BLACKKNIGHT == iter)
		{
			UClass* MonsterBPClass = StaticLoadClass(AR1Monster_BlackKnight::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Monster/BP_R1BlackKnight_Monster.BP_R1BlackKnight_Monster_C'"));
			if (MonsterBPClass)
			{
				AR1Monster_BlackKnight* MonsterInstance = GetWorld()->SpawnActor<AR1Monster_BlackKnight>(MonsterBPClass, SpawnLocation, SpawnRotation);
			}
		}
	}
	return true;
}

template <typename T>
TArray<T*> AR1Player::FindActorsByClass(UObject* WorldContextObject)
{
	TArray<T*> FoundActors;

	if (!WorldContextObject) return FoundActors;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return FoundActors;

	for (TActorIterator<T> It(World); It; ++It)
	{
		FoundActors.Add(*It);
	}
	return FoundActors;
}
template TArray<AActor*> AR1Player::FindActorsByClass<AActor>(UObject* WorldContextObject);
template TArray<AR1Monster*> AR1Player::FindActorsByClass<AR1Monster>(UObject* WorldContextObject);
template TArray<AEnvironment_Cliff*> AR1Player::FindActorsByClass<AEnvironment_Cliff>(UObject* WorldContextObject);
template TArray<ATriggerBox*> AR1Player::FindActorsByClass<ATriggerBox>(UObject* WorldContextObject);
template TArray<AR1ReconTower*> AR1Player::FindActorsByClass<AR1ReconTower>(UObject* WorldContextObject);
template TArray<AEnvironment_Cloud*> AR1Player::FindActorsByClass<AEnvironment_Cloud>(UObject* WorldContextObject);
template TArray<AR1Npc*> AR1Player::FindActorsByClass<AR1Npc>(UObject* WorldContextObject);
template TArray<ABuilding_PlayerSpawner*> AR1Player::FindActorsByClass<ABuilding_PlayerSpawner>(UObject* WorldContextObject);
template TArray<ABuilding_Telescope*> AR1Player::FindActorsByClass<ABuilding_Telescope>(UObject* WorldContextObject);

template<typename T>
TArray<UUserWidget*> AR1Player::FindWidgetsByClass(UObject* WorldContextObject)
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(WorldContextObject, FoundWidgets, T::StaticClass(), false);
	return FoundWidgets;
}

template TArray<UUserWidget*> AR1Player::FindWidgetsByClass<UMission_Widget>(UObject* WorldContextObject);
template TArray<UUserWidget*> AR1Player::FindWidgetsByClass<UMonsterHP_Widget>(UObject* WorldContextObject);
template TArray<UUserWidget*> AR1Player::FindWidgetsByClass<UPlayerBar_Widget>(UObject* WorldContextObject);
template TArray<UUserWidget*> AR1Player::FindWidgetsByClass<USecond_Widget>(UObject* WorldContextObject);
