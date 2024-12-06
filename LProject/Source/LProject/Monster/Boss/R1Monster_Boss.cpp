// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss/R1Monster_Boss.h"
#include "Player/MyPlayerState.h"
#include "Player/R1Player.h"

#include "Interactive_Buildings/Telescope/Building_TelescopeTower.h"

#include "Camera/CameraManager.h"
#include "Kismet/GameplayStatics.h" 
#include "R1GameModeBase.h"

 // 나이아가라 
#include "NiagaraFunctionLibrary.h" 
#include "NiagaraComponent.h" 
#include "NiagaraSystem.h"  

// Widget
#include "UI/InGame/Fight/MonsterEnergy_Widget.h"
#include "UI/InGame/Fight/MonsterHP_Widget.h"
#include "UI/R1UserWidget.h"
#include "Components/TextBlock.h"


#define ATTACK_CNT			0 // 어택 횟수
#define ATTACK_TERM		2.f // 어택 텀

AR1Monster_Boss::AR1Monster_Boss()
	: Super()
{
	// Hidden Attack Particle
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraAsset(TEXT("NiagaraSystem'/Game/Art/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Maniac_Make.NS_RocketExhaust_Maniac_Make'"));
	if (NiagaraAsset.Succeeded())
	{
		HiddenAttack_Particle = NiagaraAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AR1Monster_Boss : Failed to load Niagara Particle"));
	}

	// Attack Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DAParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_IceShard_01.Par_LoPo_IceShard_01'"));
	if (DAParticleAsset.Succeeded())
		Attack_Particle = DAParticleAsset.Object;
	else
		MSG_BOX(TEXT("no Rwput_ReadyParticle Create"));

	// Rwput Attack Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_IceSpikes_03.Par_LoPo_IceSpikes_03'"));
	if (ParticleAsset.Succeeded())
		Rwput_Particle = ParticleAsset.Object;
	else
		MSG_BOX(TEXT("no Rwput_Particle Create"));

	// Rwput Attack Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> RwParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Art/LoPoPack/Particles/Par_LoPo_IceTriStar_01.Par_LoPo_IceTriStar_01'"));
	if (RwParticleAsset.Succeeded())
		Rwput_ReadyParticle = RwParticleAsset.Object;
	else
		MSG_BOX(TEXT("no Rwput_ReadyParticle Create"));

}

void AR1Monster_Boss::BeginPlay()
{
	Setting_MonsterAbility(9);

	if (!Setting_MonsterInfo())
		MSG_BOX(TEXT("AR1Monster_Boss() : 몬스터의 정보가 세팅되지 않았습니다."));

	if (!Setting_Widget())
		MSG_BOX(TEXT("AR1Monster_Boss() : Widget이 세팅되지 않았습니다."));

	Super::BeginPlay();

	Boss_InitPosition = GetActorLocation();
}

void AR1Monster_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PlayerTarget_Setting())
	{
		if (true == bBossStart)
		{
			// Attack
			Select_Attack();
			Pattern_Function();
			Impact_PlayerPattern();

			// Widget
			CrosshairWidget();
		}
	}
}

bool AR1Monster_Boss::Pattern_Function()
{
	if (nullptr == CurrentPlayerState)
		return false;

	if (true == bDead)
		return false;

	if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::DEAD)])
	{
		bDead = Drop_Coin(this->GetActorLocation(), true, R1Player->GetActorLocation());
		return false;
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)])
	{
		if (true == Hit())
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = false;
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)])
	{
		if (Hidden_Attack())
		{
			bTargetPos = false;
			HiddenAttack_Timer = 0.f;
			TargetPos_Timer = 0.f;

			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)] = false;
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = true;
		}
	}
	
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ROWPUT_ATTACK)])
	{
		if (RowPut_Attack())
		{
			ReadyRowPut_Timer = 0.f;
			bReadyRowPut_Position = false;
			bRowPutAttack_End = false;

			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ROWPUT_ATTACK)] = false;
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = true;
		}
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)])
	{
		if (AroundBuilding())
			Stop_AroundBuilding();
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::AIM_TELEPORT)])
	{
		// 1. Aim 중일 때
		if (R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)])
		{
			
		}
		// 2. Aim Attack의 순간
		else if (R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM_ATTACK)])
		{

		}
	}
		
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::READY_FIGHT)])
	{
		if (Ready_Levitation())
		{
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::READY_FIGHT)] = false;
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = true;
		}
	}
	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HILL)])
	{
	}

	else if (true == Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)])
	{
		if (Default_StartAttack())
		{
			++ParticleAttack_Idx;
			if (ParticleAttack_Idx > 3)
			{
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = false;
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = true;

				ParticleAttack_Idx = 0;
				DefaultAttack_Distance = 2000.f;
			}
		}
	}
	return true;
}

void AR1Monster_Boss::Impact_PlayerPattern()
{
	if (false == IsMonsterTurn())
		return;

	if (true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIDDEN_ATTACK)] ||
		true == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::ATTACK)])
	{
		if (EIMPACT::IMPACT_ATTACK != IsImpact)
		{
			IsImpact = EIMPACT::IMPACT_ATTACK;

			/*if (true == CalculateAbility((ABILITY_TYPE::DEFENCE)))
			{
				MSG_BOX(TEXT("MONSTER 회피 성공"));
			}
			else*/
			{
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIT)] = true;
			}
		}
	}
	else
		IsImpact = EIMPACT::IMPACT_END;
}

bool AR1Monster_Boss::Setting_MonsterInfo()
{
	MonsterName = FText::FromString(TEXT("북쪽 마법사"));
	MonsterType = EMONSTER_TYPE::BOSS;
	SetActorEnableCollision(false);

	BossSkeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	return true;
}

bool AR1Monster_Boss::Setting_Widget()
{
	Crosshair_Instance = Cast<UR1UserWidget>(CreateWidget(GetWorld(), Crosshair_Widget));
	if (Crosshair_Instance)
	{
		Crosshair_Instance->AddToViewport();
		Crosshair_Instance->Set_ShowWidget(false);
	}
	else
		return false;

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
	}
	else
		return false;

	/*  2. Monster Energy Widget */
	MonsterEnergy_Instance = Cast<UMonsterEnergy_Widget>(CreateWidget(GetWorld(), MonsterEnergy_Widget));
	if (MonsterEnergy_Instance)
	{
		MonsterEnergy_Instance->AddToViewport();
		MonsterEnergy_Instance->Set_Monster(this);
	}
	else
		return false;

	return true;
}

void AR1Monster_Boss::CrosshairWidget()
{
	if (nullptr == Crosshair_Instance)
		return;

	if(ESlateVisibility::Hidden == Crosshair_Instance->Get_ShowWidget())
	{
		if (R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)])
			Crosshair_Instance->Set_ShowWidget(true);
	}
	else if (ESlateVisibility::Visible == Crosshair_Instance->Get_ShowWidget())
	{
		CrosshairHideAndBlink();
	}
}

void AR1Monster_Boss::CrosshairScore()
{
	if (ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager))
	{
		// 아직 Player Aim 공격이 끝나지 않았음
		if (false == MyCameraManager->Get_AimAttackEnd())
		{
			if (UTextBlock* ScoreText = Cast<UTextBlock>(Crosshair_Instance->GetWidgetFromName(TEXT("Score_Text"))))
			{
				if (ESlateVisibility::Visible == ScoreText->GetVisibility())
					ScoreText->SetVisibility(ESlateVisibility::Hidden);
			}

			if (UTextBlock* PercentText = Cast<UTextBlock>(Crosshair_Instance->GetWidgetFromName(TEXT("Percent_Text"))))
			{
				FString AimString = FString::Printf(TEXT("%d%"), AimPercent);
				PercentText->SetText(FText::FromString(AimString));
			}
		}
		// 끝났음
		else
		{
			if (UTextBlock* ScoreText = Cast<UTextBlock>(Crosshair_Instance->GetWidgetFromName(TEXT("Score_Text"))))
			{
				if (ESlateVisibility::Hidden == ScoreText->GetVisibility())
					ScoreText->SetVisibility(ESlateVisibility::Visible);

				if (UTextBlock* PercentText = Cast<UTextBlock>(Crosshair_Instance->GetWidgetFromName(TEXT("Percent_Text"))))
					PercentText->SetText(FText::FromString(TEXT("")));

				if (AimPercent >= 100)
					ScoreText->SetText(FText::FromString(TEXT("MASTER!")));
				else if (AimPercent >= 97)
					ScoreText->SetText(FText::FromString(TEXT("PERPECT!")));
				else if (AimPercent >= 93)
					ScoreText->SetText(FText::FromString(TEXT("GOOD")));
				else
					ScoreText->SetText(FText::FromString(TEXT("BAD")));

				if(bool bResult = AimPercent < 93)
					CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM_ATTACK)] = true;

				bAimEnd = true;
			}
		}
	}
}

void AR1Monster_Boss::CrosshairHideAndBlink()
{
	if (false == bAimEnd)
	{
		CrosshairScore();
	}
	else if (true == bAimEnd)
	{
		ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
		
		// 1. 아직 에임이 꺼지지 않았다면, 
		if (true == MyCameraManager->Get_AimAttackEnd())
		{
			// ON
			if (false == bAimScoreBlinking)
			{
				AimScoreBlinking_Timer += GetWorld()->GetDeltaSeconds();
				if (AimScoreBlinking_Timer > 1.f)
				{
					AimScoreBlinking_Timer = 0.f;
					bAimScoreBlinking = true;
				}
			}
			// OFF
			else if (true == bAimScoreBlinking)
			{
				AimScoreBlinking_Timer += GetWorld()->GetDeltaSeconds();
				if (AimScoreBlinking_Timer > 0.4f)
				{
					AimScoreBlinking_Timer = 0.f;
					bAimScoreBlinking = false;
				}
			}
		}
		else // 2. 에임이 꺼졌다면
		{
			AimScoreBlinking_Timer = 0.f;
			bAimScoreBlinking = false;
			bAimEnd = false;
			Crosshair_Instance->Set_ShowWidget(false);
		}
	}
}

bool AR1Monster_Boss::PlayerTarget_Setting()
{
	if (true == bBossStart)
		return true;

	if (EFIGHT_AUTHORITY::PLAYER == R1Player->Get_FightAuthority())
	{
		return false;
	}

	ACameraManager* MyCameraManager = Cast<ACameraManager>(GetWorld()->GetFirstPlayerController()->PlayerCameraManager);
	// 1. 보스 맵일 때 보스를 타겟 몬스터로 잡는다.
	if (this != CurrentPlayerState->TargetMonster)
	{
		if (EBOSSMAP_CAMERA::BOSS_SCENE_BC == MyCameraManager->Get_BCCameraIndex())
		{
			if (AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this)))
			{
				for (auto iter : GameMode->PlayerArray)
					iter->Get_CurrentPlayerState()->TargetMonster = this;
			}
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuilding_TelescopeTower::StaticClass(), FoundActors);

	// 2. 센터 가져오기, 공중 부양
	if (3 <= MyCameraManager->Get_BCSecondCutScene_CameraIndex())
	{
		if (FoundActors.Num() > 0)
		{
			ABuilding_TelescopeTower* MyBuilding_TelescopeTower = Cast<ABuilding_TelescopeTower>(FoundActors[0]);
			if (MyBuilding_TelescopeTower)
			{
				Building_Center = MyBuilding_TelescopeTower->Get_Center();
				bBossStart = true;

				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::READY_FIGHT)] = true;
				return true;
			}
		}
	}
	return false;
}

bool AR1Monster_Boss::Ready_Levitation()
{
	FVector CurrentPos = this->GetActorLocation();
	FVector TargetPos = Boss_InitPosition;
	TargetPos.Z += 400.f;

	FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, GetWorld()->GetDeltaSeconds(), 2.f);

	this->SetActorLocation(NewPos);

	if (10.f >= FVector::Dist(NewPos, TargetPos))
		return true;

	return false;
}

bool AR1Monster_Boss::RowPut_Attack()
{
	ReadyRowPut_Timer += GetWorld()->GetDeltaSeconds();
	if(false == bRowPutAttack_End)
	{
		FVector PlayerPos = R1Player->GetActorLocation();
		PlayerPos.Z += 88.f;

		// 1. 시작
		if (false == bReadyRowPut_Position)
		{
			bReadyRowPut_Position = true;
			ReadyRowPut_Timer = 0.f;

			FVector TargetPos = PlayerPos;
			TargetPos.Z += 5000.f;
			this->SetActorLocation(TargetPos);

			Rwput_ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Rwput_ReadyParticle, R1Player->GetActorLocation(), FRotator::ZeroRotator, true);

			FVector NewScaled(3.f, 3.f, 3.f);
			Rwput_ParticleComponent->SetWorldScale3D(NewScaled);
		}

		else if (true == bReadyRowPut_Position)
		{
			if (ReadyRowPut_Timer < 1.5f)
			{
				Rwput_ParticleComponent->SetWorldLocation(R1Player->GetActorLocation());
				Boss_TargetPosition = R1Player->GetActorLocation();
			}

			if (ReadyRowPut_Timer >= 2.5f)
			{
				FVector BossPos = this->GetActorLocation();
				FVector NewPos = FMath::VInterpTo(BossPos, Boss_TargetPosition, GetWorld()->GetDeltaSeconds(), 15.f);
				this->SetActorLocation(NewPos);
				
				// 만약 목표에 도달한다면 
				if (120.f >= FVector::Dist(NewPos, Boss_TargetPosition))
				{ 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Rwput_Particle, this->GetActorLocation(), FRotator::ZeroRotator, true);
					
					FVector CenterAttackLoc = this->GetActorLocation();

					// Particle
					const int Distance = 700.f;
					for (int32 i = 0; i < 8; ++i)
					{
						// 1. Spawn Particle
						float Angle = (i * 45.0f) * (PI / 180.0f); 
						FVector Direction = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f); 
						FVector SpawnLocation = CenterAttackLoc + (Direction * Distance);

						// 2. Translate Scaled
						UParticleSystemComponent* SpawnAttack_Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Rwput_Particle, SpawnLocation, FRotator::ZeroRotator, true);
						FVector ParticleScaled = SpawnAttack_Particle->GetRelativeScale3D();
						ParticleScaled.X += 1.0f;
						ParticleScaled.Y += 1.0f;
						ParticleScaled.Z += 1.0f;
						SpawnAttack_Particle->SetRelativeScale3D(ParticleScaled);

						// 3. Collision
						SpawnAttack_Particle->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
						SpawnAttack_Particle->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
					}

					FVector NewScaled(3.f, 3.f, 3.f);
					bRowPutAttack_End = true;
					ReadyRowPut_Timer = 0.f;
					Rwput_ParticleComponent->DestroyComponent();
				}
			}
		}
	}
	else
	{
		// HP 충돌 처리
		if (40.f >= FVector::Dist(R1Player->GetActorLocation(), GetActorLocation()))
		{
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = true;
		}

		FVector BossPos = this->GetActorLocation();
		FVector TargetPos = BossPos;
		TargetPos.X -= Speed; // X축 이동
		TargetPos.Z += Speed; // Z축 이동

		FVector NewPos = FMath::VInterpTo(BossPos, TargetPos, GetWorld()->GetDeltaSeconds(), 10.f);
		this->SetActorLocation(NewPos);

		if (ReadyRowPut_Timer >= 0.1f)
			return true;
	}
	return false;
}

bool AR1Monster_Boss::AroundBuilding()
{
	/* 1. 회전*/
	FVector PlayerFromDistance = (R1Player->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
	this->SetActorRotation(PlayerFromDistance.Rotation());

	/* 2. 위치 회전*/
	FVector CenterLocation = Building_Center;
	FVector CurrentLocation = GetActorLocation();
	FVector Direction = CurrentLocation - CenterLocation;

	FRotator RotationDelta(0.f, RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.f);
	Direction = RotationDelta.RotateVector(Direction);
	Direction.Normalize();
	Direction *= BetweenBuilding_Distance;

	FVector NewLocation = CenterLocation + Direction;
	NewLocation.Z = Boss_InitPosition.Z - 50.f;
	
	FVector NewAroundTargetPos = FMath::VInterpTo(CurrentLocation, NewLocation, GetWorld()->GetDeltaSeconds(), 4.f);
	
	SetActorLocation(NewAroundTargetPos);

	// 일정 시간이 지나면
	ReadyAround_Timer += GetWorld()->GetDeltaSeconds();
	if (ReadyAround_Timer >= ATTACK_TERM)
	{
		return true;
	}
	return false;
}

void AR1Monster_Boss::Stop_AroundBuilding()
{
	ReadyAround_Timer = 0.f;
	bAroundCenter = false;
	bSelect_AttackType = true;
}

void AR1Monster_Boss::Select_Attack()
{
	if (EFIGHT_AUTHORITY::MONSTER != R1Player->Get_FightAuthority())
	{
		// 기본 Aim 시 => 텔레포트, Aim Attack 시 고정
		if (true == R1Player->Get_CurrentPlayerState()->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::AIM)])
		{
			Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::AIM_TELEPORT)] = true;
		}

		PrevAuthority = EFIGHT_AUTHORITY::PLAYER;
		return;
	}

	if(PrevAuthority != R1Player->Get_FightAuthority())
	{ 
		AttackCnt = 0;
		// Boss가 도는 값이 쌓이기 때문에 한 번 초기화한다.
		Stop_AroundBuilding();
		bSelect_AttackType = false;

		PrevAuthority = R1Player->Get_FightAuthority();
	}

	if (true == bSelect_AttackType)
	{
		if (ATTACK_CNT < AttackCnt)
		{
			// 플레이어를 변경할 때 공격이 끝나고 1.5초 뒤에 변경된다.
			AttackEnd_Timer += GetWorld()->GetDeltaSeconds();
			if (AttackEnd_Timer >= 1.5f)
			{
				AttackEnd_Timer = 0.f;
				R1Player->Set_FightAuthority(EFIGHT_AUTHORITY::PLAYER);
				Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = true;

				//  플레이어로 넘긴다.
				R1Player->Set_BossMap_PlayerChange();
			}
		}
		else
		{
			if (true == MonsterEnergy_Instance->Get_MonsterAttack_Start())
			{
				++AttackCnt;
				if (CalculateAbility(ABILITY_TYPE::POWER))
				{
					bSelect_AttackType = false;
					Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::HIDDEN_ATTACK)] = true;
					Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = false;
				}
				else
				{
					bDefaultAttack = !bDefaultAttack;
					if (false == bDefaultAttack)
					{
						bSelect_AttackType = false;
						Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ATTACK)] = true;
						Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = false;
					}
					else
					{
						bSelect_AttackType = false;
						Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::ROWPUT_ATTACK)] = true;
						Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::MOVING)] = false;
					}
				}
			}
		}
	}
}

bool AR1Monster_Boss::Hidden_Attack()
{
	FVector CurrentPos = GetActorLocation();
	FVector PlayerLot = R1Player->GetActorLocation();

	if (!bTargetPos)
	{
		bTargetPos = true;
		FVector Normal = (Player_LastPos - CurrentPos).GetSafeNormal();
		Boss_TargetPosition = CurrentPos + (Normal * 100.f);
		Boss_TargetPosition.Z = Boss_InitPosition.Z + 1000.f; // Z축 고정

		// 파티클 생성
		if (HiddenAttack_Particle)
			NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HiddenAttack_Particle, GetActorLocation(), GetActorRotation());
	}

	/* 0.5초마다 Player 위치 판별*/
	TargetPos_Timer += GetWorld()->GetDeltaSeconds();
	if (TargetPos_Timer >= 0.5f)
	{
		TargetPos_Timer = 0.f;
		Player_LastPos = PlayerLot;
		Player_LastPos.Z += 88.f;
	}

	// 1. 보스 위치 변경
	FVector NewPos = FMath::VInterpTo(CurrentPos, Boss_TargetPosition, GetWorld()->GetDeltaSeconds(), 2.f);
	SetActorLocation(NewPos);

	// 2. 나이아가라 파티클 회전 변경
	FVector NewNiaRot = (Player_LastPos - GetActorLocation()).GetSafeNormal();
	if(NiagaraComponent)
	{
		FRotator CurrentNiagaraRot = NiagaraComponent->GetComponentRotation();
		FRotator NewNiagaraRot = FMath::RInterpTo(CurrentNiagaraRot, NewNiaRot.Rotation(), GetWorld()->GetDeltaSeconds(), 0.5f);

		NiagaraComponent->SetWorldRotation(NewNiagaraRot);
		NiagaraComponent->SetWorldLocation(GetActorLocation());
	}

	const float PlayerRadius = 1.0f;
	FVector NiaPos = NiagaraComponent->GetComponentLocation();
	FVector NiaDistancePos = (PlayerLot - NiaPos); // 이펙트와 Player 거리
	float distanceToPlayer = NiaDistancePos.Length(); // 플레이어와의 거리
	float projection = NewNiaRot.Dot(NiaDistancePos); // 방향 벡터와 내적 

	// 두 벡터가 같은 방향을 가리킬 때 양수 / 만약 범위 안에 들어간다면
	if (projection >= 0.f && PlayerRadius >= abs(distanceToPlayer - projection))
	{
		/* 플레이어와 같은 위치의 점을 구함 */
		float Distance = FVector::Dist(CurrentPos, PlayerLot);
		FVector Target = CurrentPos + (NewNiaRot * Distance);

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%f"), (Target.Z - PlayerLot.Z)));
		if(10.f >= (Target.Z - PlayerLot.Z) && -10.f <= (Target.Z - PlayerLot.Z))
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = true;
	}

	/* 6 초 이상 견디면 다음 턴*/
	HiddenAttack_Timer += GetWorld()->GetDeltaSeconds();
	if (HiddenAttack_Timer >= 6.f)
	{
		NiagaraComponent->Deactivate();
		return true;
	}
	return false;
}

bool AR1Monster_Boss::Default_StartAttack()
{
	 // 1. 파티클 생성
	if (false == bSpawnAttackParticle)
	{
		// 1. 생성할 위치 지정하기
		if (false == bTargetPos)
		{
			bTargetPos = true;
			Player_LastPos = R1Player->GetActorLocation();
		}

		// 2. Spawn
		float SpawnAngle = (360.f / 5.f) * (Array_AttackParticle.Num() + 1);
		FVector SpawnLocation = Player_LastPos + FVector(DefaultAttack_Distance * FMath::Cos(FMath::DegreesToRadians(SpawnAngle)), DefaultAttack_Distance * FMath::Sin(FMath::DegreesToRadians(SpawnAngle)), 0.f);
		FRotator SpawnRotation = FRotator(0.f, SpawnAngle, 0.f);

		UParticleSystemComponent* DAParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Attack_Particle, SpawnLocation, SpawnRotation, true);

		// 3. Collision
		DAParticleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		DAParticleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Array_AttackParticle.Emplace(DAParticleComponent);
		
		if (Array_AttackParticle.Num() >= 5)
		{
			bSpawnAttackParticle = true;
			bTargetPos = false;
		}

		Boss_TargetPosition = this->GetActorLocation();
		Boss_TargetPosition.Z += 50.f;
	}
	
	 // 몬스터 위치 지속
	 FVector NewMonsterPos = FMath::VInterpTo(this->GetActorLocation(), Boss_TargetPosition, GetWorld()->GetDeltaSeconds(), 3.f);
	 this->SetActorLocation(Boss_TargetPosition);

	for (int i = 0; i < Array_AttackParticle.Num(); i++)
	{
		// Player 위치로 이동
		FVector ParticlePos = Array_AttackParticle[i]->GetComponentLocation();
		FVector TargetPos = Player_LastPos;/* R1Player->GetActorLocation();*/
		FVector NewPos = FMath::VInterpTo(ParticlePos, TargetPos, GetWorld()->GetDeltaSeconds(), 2.f);
		Array_AttackParticle[i]->SetWorldLocation(NewPos);

		// Player 충돌
		if (100.f >= FVector::Dist(Array_AttackParticle[i]->GetComponentLocation(), TargetPos))
		{
			CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)] = true;
		}
	}

	// 파티클 공격 끝
	DefaultParticle_Timer += GetWorld()->GetDeltaSeconds();
	if (nullptr != Array_AttackParticle[0] && 20.f >= FVector::Dist(Player_LastPos, Array_AttackParticle[0]->GetComponentLocation()))
	{
		DefaultParticle_Timer = 0.f;

		for (int i = 0; i < Array_AttackParticle.Num(); i++)
			Array_AttackParticle[i]->Deactivate();
		Array_AttackParticle.Empty();

		DefaultAttack_Distance += 300.f;
		bSpawnAttackParticle = false;
		return true;
	}
	return false;
}
