

#include "Monster/R1Monster.h"
#include "Components/CapsuleComponent.h"
#include "Player/R1Player.h"
#include "Player/MyPlayerState.h"

#include "R1GameModeBase.h"

/* Widget */
#include "UI/InGame/Fight/ReadyFight_Widget.h"
#include "UI/InGame/Fight/MonsterHP_Widget.h"
#include "UI/InGame/Fight/MonsterEnergy_Widget.h"
#include "UI/InGame/Fight/RewardItem/RewardItem_Widget.h"
#include "Components/WidgetComponent.h"

/* Particle */
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h" 
#include "Particles/ParticleSystemComponent.h" 

#include "System/R1GameInstance.h"
#include "Animation/AnimInstance.h"
#include "Animation/Monster_Anim/Monster_AnimInstance.h"

#define CONNECT_DISTANCE 500.f

AR1Monster::AR1Monster()
    : Super()
{
	PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 교차 오버랩
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapAll"));
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Pawn과 오버랩
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AR1Monster::OnBeginOverlap);

    // 3D Widget
    CurrentState_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MONSTER_3DWIDGET"));
    CurrentState_Widget->SetupAttachment(GetMesh());
    CurrentState_Widget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
    CurrentState_Widget->SetWidgetSpace(EWidgetSpace::Screen);

    static ConstructorHelpers::FClassFinder<UUserWidget> CurrentWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/InFight/WBP_FightCurrentState.WBP_FightCurrentState_C'"));
    if (CurrentWidget.Succeeded())
    {
        CurrentState_Widget->SetWidgetClass(CurrentWidget.Class);
        CurrentState_Widget->SetDrawSize(FVector2D(1000, 1000));
    }

    // 1. Particle : Hill
    static ConstructorHelpers::FObjectFinder<UParticleSystem> Hill_PS(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Shield_01.Par_LoPo_Shield_01'"));
    if (Hill_PS.Succeeded())
        Hill_Particle = Hill_PS.Object;

    // 2. Particle : 등장 먹구름
    static ConstructorHelpers::FObjectFinder<UParticleSystem> Appearance_ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_Clouds_01.Par_LoPo_Clouds_01'"));
    if (Appearance_ParticleSystem.Succeeded())
        Appearance_Particle = Appearance_ParticleSystem.Object;
}

void AR1Monster::BeginPlay()
{
	Super::BeginPlay();

    if (!Setting_Item())
        MSG_BOX(TEXT("AR1Monster() : Item을 찾을 수 없음"));
   
    if (!Find_Player())
        MSG_BOX(TEXT("AR1Monster() : 플레이어를 찾을 수 없음"));

    if (!Find_Widget())
        MSG_BOX(TEXT("AR1Monster() : 위젯을 찾을 수 없음"));

    if(!Setting_Animation())
        MSG_BOX(TEXT("AR1Monster() : 애니메이션을 찾을 수 없음"));
 
    if(!Setting_Material())
        MSG_BOX(TEXT("AR1Monster() : 머터리얼을 찾을 수 없음"));

    Origin_Scaled = this->GetActorScale3D();
    Target_Scaled = Origin_Scaled;
    Target_Scaled.X -= 0.3;
    Target_Scaled.Y -= 0.3;
}

bool AR1Monster::Pattern_Function()
{
    if (false == IsMonsterTurn())
        return false;

    if (true == bDead)
    {
        if (true == bFightActityStop)
        {
            if (true == bFightEnd)
                return false;

            if (EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
                Ready_NextTurn(EMONSTER_PATTERN::HILL, EFIGHT_AUTHORITY::PLAYER);
        }
        else
        {
            /* 몬스터가 전부 죽지 않았다면 계속 게임을 진행한다.*/
            TArray<AR1Monster*> MonsterArray;
            UObject* WorldContextObject = GetWorld();
            MonsterArray = AR1Player::FindActorsByClass<AR1Monster>(WorldContextObject);
            for (auto iter : MonsterArray)
            {
                if (false == iter->Get_Monster_Pattern()[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
                    return false;
            }

            /* 플레이어가 이겼다. result Fight Widget 불러오기*/
            bFightEnd = true;
            R1Player->Set_RidAllMonster(); // 모든 몬스터를 처치하였다.
        }
        return false;
    }
    return true;
}

void AR1Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!Find_Player())
        MSG_BOX(TEXT("플레이어를 찾을 수 없음 : Monster"));

    if (nullptr == CurrentPlayerState)
        return;

	if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene)
	{
        // 1. 때릴 때 이미시브 밝기 Up
        if (true == bEmissive)
            Change_Emissive();

        //2. 몬스터 작동
        Operate_FightScene();

        if (true == bFightEnd) // 마지막 Monster라서 Widget을 열겠다.
        {
            if (true == bFightActityStop) // 모션이 지난 후로 3초 뒤에
            {
                RewardWidget_Timer += DeltaTime;
                if (RewardWidget_Timer >= 3.f)
                {
                    R1Player->Set_EndFight(true);
                    bFightEnd = false;
                }
            }
        }
	}
	else if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
        Lookat_Player();
	}
    
}

void AR1Monster::Operate_FightScene()
{
    // Monster 차례에 공격을 타입을 정해준다.
    Attack_Type();

    Monster_Dead();
    MonsterSpawn();

    /* 몬스터의 크기 : 맞을 때마다 XY 축이 줄어듦*/
    if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] && false == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
    {
        FVector NewScaled = FMath::VInterpTo(this->GetActorScale3D(), Target_Scaled, GetWorld()->GetDeltaSeconds(), 4.f);
        this->SetActorScale3D(NewScaled);
    }
    else
    {
        FVector NewScaled = FMath::VInterpTo(this->GetActorScale3D(), Origin_Scaled, GetWorld()->GetDeltaSeconds(), 2.f);
        this->SetActorScale3D(NewScaled);
    }
}

void AR1Monster::Setting_MonsterAbility(int32 _level)
{
    int _hp, _attack, _hill;
    switch (_level)
    {
    case 1 :
    {
        _hp = 60;
        _attack = 20;
        _hill = 10;
    }
        break;
    case 2 :
    {
        _hp = 100;
        _attack = 30;
        _hill = 15;
    }
        break;
    case 3 :
    {
        _hp = 140;
        _attack = 35;
        _hill = 20;
    }
        break;
    case 4 :
    {
        _hp = 170;
        _attack = 45;
        _hill = 25;
    }
        break;
    case 5 :
    {
        _hp = 210;
        _attack = 60;
        _hill = 30;
    }
        break;
    case 6 :
    {
        _hp = 250;
        _attack = 70;
        _hill = 35;
    }
        break;
    case 7 :
    {
        _hp = 300;
        _attack = 85;
        _hill = 35;
    }
        break;
    case 8 :
    {
        _hp = 400;
        _attack = 100;
        _hill = 40;
    }
        break;
    case 9:
    {
        _hp = 3000;
        _attack = 300;
        _hill = 300;
    }
    break;
    }
    Hp = CurrentHp = MaxHp = _hp;
    CurrentAttack = TargetAttack = _attack;
    FinalHill = TargetHill = _hill;
    CurrentLevel = _level;
}

void AR1Monster::Get_UseEnergy(bool _using)
{
    if (ReadyFightWidget)
        ReadyFightWidget->Use_Energy(_using);
}

void AR1Monster::Set_ProgressBar(float _hp)
{
    if(MonsterHPBar)
	    MonsterHPBar->Set_ProgressBar(_hp);
}

void AR1Monster::DecreaseHP(float& _currentHP, float _targetHP, float DeltaTime, float _speed)
{
    Super::DecreaseHP(_currentHP, _targetHP, DeltaTime, _speed);

    if (MonsterHPBar)
        MonsterHPBar->Set_HpText(MaxHp, CurrentHp);

    Set_ProgressBar(CurrentHp / 100.f);
}

bool AR1Monster::IsMonsterTurn()
{
    return (nullptr == CurrentPlayerState || this != CurrentPlayerState->TargetMonster) ? false : true;
}

void AR1Monster::Start_Emissive(EMONSTER_PATTERN _pattern)
{
    if (false == bEmissiveStart)
    {
        bEmissive = true;
        bEmissiveStart = true;
        EmissivePattern = static_cast<BYTE>(_pattern);
    }
}

void AR1Monster::Change_Emissive()
{
    if (true == bIncreasing)
    {
        CurrentValue += 1.0f * 2.f;
        if (CurrentValue >= 30.f)
            bIncreasing = false;
    }
    else if(false == bIncreasing)
    {
        CurrentValue -= 1.0f * 2.f;
        if (CurrentValue <= 0.0f)
        {
            CurrentValue = 0.f;
            bIncreasing = true;
            bEmissive = false;
        }
    }

    if (DynamicMaterialInstance)
        DynamicMaterialInstance->SetScalarParameterValue(FName("EmiValue"), CurrentValue);

    if(false == Monster_Pattern[static_cast<int32>(EmissivePattern)])
        bEmissiveStart = false;
}

void AR1Monster::OnBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

bool AR1Monster::Attack_Type()
{
    if(EFIGHT_AUTHORITY::PLAYER == R1Player->Get_FightAuthority() ||
        true == R1Player->Get_EndFight())
    {
        bSelect_AttackType = false;
        return false;
    }

    if (true == bSelect_AttackType)
    {
        return bSelect_AttackType;
    }

	if (MonsterEnergy_Instance)
	{
        if (true == MonsterEnergy_Instance->Get_MonsterAttack_Start())
        {
            if (EFIGHT_AUTHORITY::MONSTER == R1Player->Get_FightAuthority())
            {
                if (Hp <= MaxHp / 3.f)
                {
                    if (true == CalculateAbility(ABILITY_TYPE::HILL))
                    {
                        Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)] = true;
                    }
                    else
                    {
                        Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = true;
                    }
                    bSelect_AttackType = true;
                }
                else
                {
                    if (true == CalculateAbility(ABILITY_TYPE::POWER))
                    {
                        Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)] = true;
                        bSelect_AttackType = true;
                    }
                    else
                    {
                        Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = true;
                        bSelect_AttackType = true;
                    }
                }
            }
            else
                bSelect_AttackType = false;
        }
    }
    return bSelect_AttackType;
}

void AR1Monster::Attack_Stop()
{
    bSelect_AttackType = false;
}

// 파티클 공격 생성
bool AR1Monster::ParticleStart(UParticleSystem* _particle, bool _static, float _timer, FVector _Location, int _particleCnt)
{
    if (true == bNextTurn)
        return true;

     if (true == SpawnParticle(_particle, _Location, _static, _timer, _particleCnt))
     {
         if(true == _static)
         {
             Particle_Timer += GetWorld()->GetDeltaSeconds();

             if (Particle_Timer >= _timer)
             {
                  if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
                      R1Player->Set_ShirkEnd();

                 Particle_Timer = 0.f;
                 DestroyParticle(_particle);
                 bNextTurn = true;
                 return true;
             }
         }
     }

     if (false == _static)
     {
         if (true == Move_ParticleArray(_Location, _particleCnt))
         {
             if(true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
                 R1Player->Set_ShirkEnd();

			  Particle_Timer = 0.f;
			  bNextTurn = true;
              DestroyParticle(_particle);

			  return true;
         }
     }
    return false;
}

bool AR1Monster::SpawnParticle(UParticleSystem* _particle, FVector _Location, bool _static, float _timer, int32 _particleCnt)
{
    if (true == bParticleStart)
        return true;

    if (_particle)
    {
        if (true == _static)
        {
            bParticleStart = true;

            // 1. 위치 선정
            FVector SpawnLocation = _Location;
            SpawnLocation.Z -= 88.f;
            FRotator SpawnRotation = FRotator::ZeroRotator;
            ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _particle, SpawnLocation, SpawnRotation);

            return true;
        }

        else if (false == _static)
        {
            Particle_Timer += GetWorld()->GetDeltaSeconds();

            if (_timer <= Particle_Timer)
            {
                FRotator SpawnRotation = FRotator::ZeroRotator;
                ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _particle, this->GetActorLocation(), SpawnRotation);
                ParticleArray.Emplace(ParticleComponent);
                Particle_Timer = 0.f;
                ParticleCnt++;

                if (ParticleCnt >= _particleCnt)
                {
                    R1Player->OnDamged(CurrentAttack - 5.f, this);
                    bParticleStart = true;
                }
            }
        }
    }
    return false;
}

void AR1Monster::DestroyParticle(UParticleSystem* _particle)
{
    Particle_Timer = 0.f;
    bParticleStart = false;
    ParticleCnt = 0;

    if (ParticleComponent)
    {
        ParticleComponent->Deactivate();
        ParticleComponent = nullptr;
    }
}

bool AR1Monster::Move_ParticleArray(FVector _Location, int32 _particleCnt)
{
    TArray<UParticleSystemComponent*> ParticlesToDestroy;

    for (UParticleSystemComponent* Part : ParticleArray)
    {
        if (Part)
        {
            FVector CurrentLocation = Part->GetComponentLocation();
            FVector NewLocation = FMath::Lerp(CurrentLocation, R1Player->Get_InitPos(), 0.1f); // 0.1f는 보간 비율
            Part->SetWorldLocation(NewLocation);

            // 일단 플레이어로 고정
            if(R1Player)
            {
                float Distance = FVector::Dist(R1Player->Get_InitPos(), NewLocation);
                if (Distance <= 10.0f)
                {
                    ParticlesToDestroy.Add(Part);
                }
            }
        }
    }

	for (UParticleSystemComponent* ParticleToDestroy : ParticlesToDestroy)
	{
		if (ParticleToDestroy)
		{
			ParticleToDestroy->DestroyComponent();
			ParticleArray.Remove(ParticleToDestroy);
		}
	}

    DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());

    if (ParticleCnt >= _particleCnt && 0 >= ParticlesToDestroy.Num()&& 0 >= ParticleArray.Num())
        return true;
    else
        return false;
}


bool AR1Monster::Ready_NextTurn(EMONSTER_PATTERN _pattern, EFIGHT_AUTHORITY _nextTarget, float _delayTimer)
{
    NextTurn_ReadyTimer += GetWorld()->GetDeltaSeconds();
    if (NextTurn_ReadyTimer >= _delayTimer)
    {
        NextTurn_ReadyTimer = 0.f;
        bNextTurn = false;

        Monster_Pattern[static_cast<int32>(_pattern)] = false;
        R1Player->Next_Energy();

        R1Player->Set_FightAuthority(_nextTarget);

        return true;
    }
    return false;
}

bool AR1Monster::HitAndDash(AR1Character* _thisObj, FVector _targetPos, float _timing)
{
    bHitAndDashStart = true;
    if(false == bHitAndDash)
        bHitAndDash = Super::HitAndDash(_thisObj, _targetPos, false, _timing); // 플레이어에게 향함
    else
        HitAndDash_Timer = 0.f;
         
    if (true == bCompletedDash)
    {
        HitAndDash_Timer += GetWorld()->GetDeltaSeconds();

        /* 몬스터가 물러날 때 살짝의 텀을 주고 회피하고 싶음*/
        if(HitAndDash_Timer >= 0.25f)
        {
            if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::SHIRK)])
                R1Player->Set_ShirkEnd();
        }
    }
    return bHitAndDash;
}

void AR1Monster::Stop_HitAndDash()
{
    bHitAndDashStart = false;
    bHitAndDash = false;
}

void AR1Monster::Set_EnergyStart(bool _start)
{
    if (MonsterEnergy_Instance)
        MonsterEnergy_Instance->Set_EnergyStart(_start);
}

FItemData_Asset* AR1Monster::Get_RewardItem()
{
    /* 몬스터가 줄 수 있는 아이템을 랜덤으로 골라 던져준다. */
    if (ItemAssets.Num() == 0)
        return nullptr;

    int32 RandomItem = FMath::RandRange(0, ItemAssets.Num() - 1);

    if (nullptr != ItemAssets[RandomItem])
        return ItemAssets[RandomItem];

    return nullptr;
}

void AR1Monster::Lookat_Player()
{
	if (true == bLookAtRotation)
	{
        // 1. 플레이어 컨트롤러를 통해 플레이어의 위치를 가져옴
        if (R1Player)
        {
            AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
            R1Player = GameMode->CurrentPlayer;

            FVector PlayerLocation =R1Player->GetActorLocation();
            FRotator LookAtRotation = (PlayerLocation - this->GetActorLocation()).Rotation();

            FRotator CurrentRotation = this->GetActorRotation();
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
            this->SetActorRotation(NewRotation);
        }

        // 2. 몬스터 동료 찾기 
        if (false == bFindConnectMonster)
        {
            R1Player->Get_CurrentPlayerState()->TargetMonster = this;

            TArray<AR1Monster*> MonsterArray;
            UObject* WorldContextObject = GetWorld();
            MonsterArray = AR1Player::FindActorsByClass<AR1Monster>(WorldContextObject);

            for (auto iter : MonsterArray)
            {
                if (CONNECT_DISTANCE >= FVector::Dist(this->GetActorLocation(), iter->GetActorLocation()))
                {
                    iter->Set_LookatPlayer(true);
                    iter->Set_FindConnectMonster();
                    CurrentPlayerState->FightTurnType_Array.Emplace(iter);
                }
            }
            bFindConnectMonster = true;
        }

        // 3. 플레이어 위치 저장 : 모든 플레이어 위치를 저장해야 함
        if (AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this)))
        {
            for (int i = 0; i < LAST_PLAYER; i++)
                GameMode->PlayerArray[i]->Get_CurrentPlayerState()->LastPlayerPosition = GameMode->PlayerArray[i]->GetActorLocation();
        }
	}
    else
    {
        if (this->GetActorRotation() != Init_Rotation)
        {
            FRotator CurrentRotation = this->GetActorRotation();
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, Init_Rotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
            this->SetActorRotation(NewRotation);
        }
    }
}

void AR1Monster::MonsterSpawn()
{
    if(false == bFight_MonsterSpawn)
	{
        if (GetWorld())
        {
            Fight_MonsterSpawnTimer += GetWorld()->DeltaTimeSeconds;
        }
    
        if(Fight_MonsterSpawnTimer > 1.f)
		{
            TArray<AR1Monster*> MonsterArray;
            UObject* WorldContextObject = GetWorld();
            MonsterArray = AR1Player::FindActorsByClass<AR1Monster>(WorldContextObject);

            /* 1 Monster */
            if (MonsterArray.Num() == 1)
            {
                InitPos = FVector(InitPos.X - 800.f, InitPos.Y, InitPos.Z + 150.f);
                this->SetActorLocation(InitPos);
                bFight_MonsterSpawn = true;
            }
            /* 2 Monster */
            else if (MonsterArray.Num() == 2)
            {
                int iterCnt = { 0 };
                for (auto iter : MonsterArray)
                {
                    if(iter == this)
                    {
                        if (0 == iterCnt)
                            InitPos = FVector(InitPos.X - 800.f, InitPos.Y - 120.f, InitPos.Z + 150.f);
                        else if (1 == iterCnt)
                            InitPos = FVector(InitPos.X - 800.f, InitPos.Y + 120.f, InitPos.Z + 150.f);
                        this->SetActorLocation(InitPos);
                    }
                    ++iterCnt;
                }
                bFight_MonsterSpawn = true;
            }
            /* 3 Monster */
            else if (MonsterArray.Num() == 3)
            {
                int iterCnt = { 0 };
                for (auto iter : MonsterArray)
                {
                    if (iter == this)
                    {
                        // Monster 끼리 200 정도의 텀을 둘 것이다.
                        if (0 == iterCnt)
                            InitPos = FVector(InitPos.X - 800.f, InitPos.Y - 230.f, InitPos.Z + 150.f);
                        else if (1 == iterCnt)
                            InitPos = FVector(InitPos.X - 800.f, InitPos.Y + -30.f, InitPos.Z + 150.f);
                        else if (2 == iterCnt)
                            InitPos = FVector(InitPos.X - 800.f, InitPos.Y + 170.f, InitPos.Z + 150.f);
                        this->SetActorLocation(InitPos);
                    }
                    ++iterCnt;
                }
                bFight_MonsterSpawn = true;
            }

            if(Appearance_Particle)
            {
                FVector ParticlePos = this->GetActorLocation();
                ParticlePos.Z -= 88.f;
                UParticleSystemComponent* AppearanceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Appearance_Particle, ParticlePos, FRotator::ZeroRotator);

                if (AppearanceComp)
                    AppearanceComp->SetFloatParameter(TEXT("ParticleSize"), 2.0f);
            }
		}
	}
}

bool AR1Monster::Find_Player()
{
    AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
    if (R1Player)
    {
        R1Player = GameMode->CurrentPlayer;
        CurrentPlayerState = R1Player->Get_CurrentPlayerState();
        return true;
    }

    if (GameMode)
    {
        R1Player = GameMode->CurrentPlayer;
        if (R1Player)
            CurrentPlayerState = R1Player->Get_CurrentPlayerState();
    }

    return (R1Player != nullptr && CurrentPlayerState != nullptr) ? true : false;
}

bool AR1Monster::Find_Widget()
{
    if(CurrentPlayerState)
	{
        // 1. 3D Widget Load 
        if (CurrentState_Widget)
        {
            CurrentState_Widget->InitWidget();
            WBPCurrent_Instance = CurrentState_Widget->GetUserWidgetObject();
        }

		if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
		{
            /* 1. Monster Info Widget*/
            if (MonsterInfo_Widget)
                ReadyFightWidget = Cast<UReadyFight_Widget>(CreateWidget(GetWorld(), MonsterInfo_Widget));

			if (ReadyFightWidget)
			{
                ReadyFightWidget->AddToViewport();
				UTexture2D* LoadedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("Texture2D'/Game/Art/ClassicPixelUI/UIElements/T_options_on.T_options_on'")));

				if (LoadedTexture)
					ReadyFightWidget->ChangeTexture = LoadedTexture;
                ReadyFightWidget->Set_Monster(this);
                // CurrentPlayerState->TargetMonster = this;
			}

            return true;
		}

		else if (ESCENE::FIGHT_SCENE == CurrentPlayerState->Player_CurrentScene || ESCENE::BOSS_SCENE == CurrentPlayerState->Player_CurrentScene)
		{
            /* 0. Reward Item Bar*/
            RewardItem_Instance = Cast<URewardItem_Widget>(CreateWidget(GetWorld(), RewardItem_Widget));
            if (RewardItem_Instance)
                RewardItem_Instance->AddToViewport(1.5);
            else
                MSG_BOX(TEXT("Failed : Reward Item Widget"));

            /* 1. Monster HP Widget */
			UMonsterHP_Widget* pMonsterHP_Widget = { nullptr };
			if (MonsterHP_Widget)
				MonsterHPBar = Cast<UMonsterHP_Widget>(CreateWidget(GetWorld(), MonsterHP_Widget));

			if (MonsterHPBar)
			{
				MonsterHPBar->AddToViewport();
				MonsterHPBar->Set_TargetMonster(this);
                MonsterHPBar->Set_AttackText(CurrentAttack);
                MonsterHPBar->Set_HpText(MaxHp, CurrentHp);
                MonsterHPBar->Set_LevelText(CurrentLevel);
               // CurrentPlayerState->TargetMonster = this;
            }
            else
                MSG_BOX(TEXT("Failed : MonsterHPBar"));

            /*  2. Monster Energy Widget */
            MonsterEnergy_Instance = Cast<UMonsterEnergy_Widget>(CreateWidget(GetWorld(), MonsterEnergy_Widget));
            if (MonsterEnergy_Instance)
            {
                MonsterEnergy_Instance->AddToViewport();
                MonsterEnergy_Instance->Set_Monster(this);
            }
            else
                MSG_BOX(TEXT("Failed : MonsterEnergy_Instance"));
		}
        return (nullptr != CurrentPlayerState->TargetMonster) ? true : false;
    }
    return false;
}

bool AR1Monster::Setting_Item()
{
    /* ItemData 불러오기*/
    const FString AssetPath = TEXT("/Script/LProject.R1ItemData'/Game/Data/DA_ItemData.DA_ItemData'");
    UR1ItemData* ItemData = Cast<UR1ItemData>(StaticLoadObject(UR1ItemData::StaticClass(), nullptr, *AssetPath));

    if (ItemData)
    {
        for (const FItemData_Asset& ItemAsset : ItemData->AssetItems)
        {
            if (MonsterType == ItemAsset.MonsterType)
            {
                FItemData_Asset* NewItem = new FItemData_Asset();

                NewItem->ItemName = ItemAsset.ItemName;
                NewItem->ItemTexture = ItemAsset.ItemTexture;
                NewItem->ItemPurchasePrice = ItemAsset.ItemPurchasePrice;
                NewItem->ItemDocument = ItemAsset.ItemDocument;
                NewItem->ItemSalePrice = ItemAsset.ItemSalePrice;
                NewItem->ItemType = ItemAsset.ItemType;

                ItemAssets.Emplace(NewItem);
            }
        }
    }
    else
        return false;
    return true;
}

bool AR1Monster::Setting_Animation()
{
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
    Init_Rotation = this->GetActorRotation();

    // Animation
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        UMonster_AnimInstance* MyAnimInstance = Cast<UMonster_AnimInstance>(AnimInstance);
        if (MyAnimInstance)
            MyAnimInstance->Set_Monster(this);
    }
    else
        return false;
    return true;
}

bool AR1Monster::Setting_Material()
{
    if (GetMesh())
    {
        DynamicMaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
        if (DynamicMaterialInstance)
            GetMesh()->SetMaterial(0, DynamicMaterialInstance);
        else
            return false;
    }
    else
        return false;
    return true;
}

void AR1Monster::Monster_Dead()
{
    if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
    {
        if (R1Player)
        {
            if (ESCENE::INGAME != R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
            {
                // 만약 죽었을 때 HP 프로그래스 바가 마무리 되지 않았다면 0으로 처리해주기
                if (MonsterHPBar)
                {
                    if(0 < MonsterHPBar->Get_ProgressBar())
                        DecreaseHP(CurrentHp, 0.f, GetWorld()->GetDeltaSeconds(), 3.f);
                }
            }
        }
    }

    if (CurrentHp <= 0.f)
    {
        Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)] = true;

        UR1GameInstance* GameInstance = Cast<UR1GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GameInstance)
            GameInstance->DestroyMonster_Number.Emplace(this->MonsterNumber);
    }
}

// 맞을 때 사용
bool AR1Monster::Hit()
{
    if (false == HPAmount)
    {
        HPAmount = true;
        OnDamged(R1Player->CurrentAttack, R1Player);
    }

    DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());

    // 2.f는 소수점 오류를 방지하기 위해서
    if (CurrentHp - 2.f <= Hp)
    {
        CurrentHp = Hp;
        HPAmount = false;
        return true;
    }
    else
        return false;
}

// 힐 사용
bool AR1Monster::Hill()
{
    if (false == HPAmount)
    {
        HPAmount = true;
        OnHill(FinalHill, this);
    }
    DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());

    if (CurrentHp >= Hp)
    {
        CurrentHp = Hp;
        DecreaseHP(CurrentHp, Hp, GetWorld()->GetDeltaSeconds());
        HPAmount = false;
        return true;
    }
    else
        return false;
}
