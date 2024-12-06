#include "R1PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputMappingContext.h"
#include "R1GameModeBase.h"

#include "LProject/System/R1GameplayTags.h"
#include "LProject/System/R1AssetManager.h"
#include "LProject/System/R1InputData.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "R1Character.h"
#include "R1Player.h"
#include "Player/MyPlayerState.h"
#include "Player/Player_Herblist/R1Player_Herbalist.h"

#include "Monster/R1Monster.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

/*Cursor*/
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraManager.h"

#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Environment/Environment_Floor.h"

AR1PlayerController::AR1PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ACameraManager::StaticClass();
}

// 뒤로 점프하고 싶을 때 ( 튕길 때 )
void AR1PlayerController::BackRotation_Jump()
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		if (false == b_One_Rotation)
		{
			b_One_Rotation = true;
			BackwardDirection = -GameMode->CurrentPlayer->GetActorForwardVector().GetSafeNormal();

			if (ACharacter* MyCharacter = Cast<ACharacter>(GameMode->CurrentPlayer))
			{
				UCharacterMovementComponent* MovementComponent = MyCharacter->GetCharacterMovement();
				if (MovementComponent)
				{
					MovementComponent->StopMovementImmediately(); 
					MovementComponent->Velocity = FVector::ZeroVector; 
				}
			}
		}
		float MoveDistance = 7.f;
		FVector NewLocation = GameMode->CurrentPlayer->GetActorLocation() + BackwardDirection * MoveDistance;
		GameMode->CurrentPlayer->SetActorLocation(NewLocation);
	}
}

void AR1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const UR1InputData* InputData = UR1AssetManager::GetAssetByName<UR1InputData>("InputData"))
	{
		if (nullptr == InputData)
			return;

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			Subsystem->AddMappingContext(InputData->InputMappingContext, 0);
	}

	Setting_Player();

	// Cursor Init
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AR1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const UR1InputData* InputData = UR1AssetManager::GetAssetByName<UR1InputData>("InputData"))
	{
		if (nullptr == InputData)
			return;

		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		/* Jump Input */
		auto Action_Jump = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Jump);
		EnhancedInputComponent->BindAction(Action_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_JumpAction);

		/* Zoom Input */
		auto Action_Zoom = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Wheel);
		EnhancedInputComponent->BindAction(Action_Zoom, ETriggerEvent::Triggered, this, &ThisClass::Input_ZoomAction);

		/* Cursor Input */
		auto Action_Cursor = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Destination);
		EnhancedInputComponent->BindAction(Action_Cursor, ETriggerEvent::Started, this, &ThisClass::OnInputStarted);
		EnhancedInputComponent->BindAction(Action_Cursor, ETriggerEvent::Triggered, this, &ThisClass::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(Action_Cursor, ETriggerEvent::Completed, this, &ThisClass::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(Action_Cursor, ETriggerEvent::Canceled, this, &ThisClass::OnSetDestinationReleased);
		
		/* Input_Action_KeyBoardMove */
		auto KeyBoard_Moving = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_KeyBoardMove);
		EnhancedInputComponent->BindAction(KeyBoard_Moving, ETriggerEvent::Triggered, this, &ThisClass::Input_KeyBoardMovingAction);
		
		/* Energy Input */
		auto Action_Energy = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Energy);
		EnhancedInputComponent->BindAction(Action_Energy, ETriggerEvent::Started, this, &ThisClass::Input_EnergyAction);

		/* Inventroy Input */
		auto Action_Inven = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Inventory);
		EnhancedInputComponent->BindAction(Action_Inven, ETriggerEvent::Started, this, &ThisClass::Input_InventoryAction);
	}
}

void AR1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if(Setting_Player())
	{
		if (false == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::HIT)])
			b_One_Rotation = false;

		// 1. 마우스 트레이싱
		TickCursorTrace();

		// 2. Jump 파티클
		if (R1Player->bPressedJump != bPrevJump)
		{
			UCharacterMovementComponent* MovementComponent = R1Player->GetCharacterMovement();
			if (MovementComponent->IsMovingOnGround())
			{
				R1Player->Spawn_JumpParticle();
				bPrevJump = R1Player->bPressedJump;
			}
		}
	}
}

bool AR1PlayerController::Setting_Player()
{
	if (R1Player)
	{
		CurrentPlayerState = R1Player->Get_CurrentPlayerState();
		return true;
	}

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	R1Player = GameMode->CurrentPlayer;
	if(R1Player)
		CurrentPlayerState = R1Player->Get_CurrentPlayerState();

	if(nullptr == R1Player || nullptr == CurrentPlayerState)
		return false;

	return true;
}

void AR1PlayerController::HandleGameplayEvent(FGameplayTag EventTag)
{
	if (EventTag.MatchesTag(R1GameplayTags::Event_Montage_Attack))
	{
		// 접촉 시에
	}
}

void AR1PlayerController::Input_JumpAction(const FInputActionValue& InputValue)
{
	if (AR1Character* myPlayer = Cast<AR1Character>(GetPawn()))
	{
		myPlayer->Jump();
		bPrevJump = myPlayer->bPressedJump;
	}
}

void AR1PlayerController::Input_ZoomAction(const FInputActionValue& InputValue)
{
	UCameraComponent* myCamera = {};

	if (R1Player)
		 myCamera = R1Player->FindComponentByClass<UCameraComponent>();

	if (myCamera)
	{
		float NewFOV = myCamera->FieldOfView - InputValue.Get<float>() * ZoomSpeed;
		myCamera->SetFieldOfView(FMath::Clamp(NewFOV, MinFOV, MaxFOV));
	}
}

void AR1PlayerController::Input_KeyBoardMovingAction(const FInputActionValue& Value)
{
	if (false ==  CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::KEYBOARD_MOVING)])
		return;

	APawn* ControlledPawn = GetPawn();

	if(ControlledPawn)
	{
		FVector Direction = FVector(Value[0], Value[1], 0.f); 
		Direction.Normalize(); 
		ControlledPawn->AddMovementInput(Direction);
	}
}

void AR1PlayerController::Input_EnergyAction(const FInputActionValue& InputValue)
{
	if (CurrentPlayerState->Player_CurrentScene == ESCENE::LOBBY)
		return;

	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if(GameMode->CurrentPlayer)
		GameMode->CurrentPlayer->Use_PlayerEnergy(true);
}

void AR1PlayerController::Input_InventoryAction(const FInputActionValue& InputValue)
{
	AR1GameModeBase* GameMode = Cast<AR1GameModeBase>(UGameplayStatics::GetGameMode(this));
	if(false == GameMode->CurrentPlayer->Get_OpenInventory())
		GameMode->CurrentPlayer->Set_OpenInventory(true);

	else
		GameMode->CurrentPlayer->Set_OpenInventory(false);
}

void AR1PlayerController::OnInputStarted()
{
	if(R1Player)
	{
		FHitResult OutCursorHit;
		if (false == GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, OUT OutCursorHit)) // 충돌 관련 부분
			return;

		if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
		{
			if(AR1Character* LocalHighlightActor = Cast<AR1Character>(OutCursorHit.GetActor()))
			{
				ChaseLobbyInteractive(LocalHighlightActor);
			}
		}
		else if(ESCENE::FIGHT_SCENE == R1Player->Get_CurrentPlayerState()->Player_CurrentScene)
		{
			if (AR1Character* LocalHighlightActor = Cast<AR1Character>(OutCursorHit.GetActor()))
			{
				ChaseTargetAndAttack(LocalHighlightActor);
			}
		}
	}
	else
	{
		StopMovement(); // 길찾기를 하다가 멈출 것이라는 예상

		if (false == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)])
			return;

		bMousePressed = true;
		TargetActor = HighlightActor;
	}
}

// 누르는 매 프레임마다 실행된다.
void AR1PlayerController::OnSetDestinationTriggered()
{
	if (EMonsterState::SKILL == Get_MonsterState())
	{
		return;
	}

	if (false == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)])
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();

	if (TargetActor) 
	{
		return;
	}

	FollowTime += GetWorld()->GetDeltaSeconds();

	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, OUT Hit); 

	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	if (nullptr != ControlledPawn)
	{
		if(false== R1Player->Get_Jumping())
		{
			FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection, 1.0f, false);
		}
	}
}

// 마우스를 떼는 순간
void AR1PlayerController::OnSetDestinationReleased()
{
	bMousePressed = false;

	if (false == CurrentPlayerState->Player_Pattern[static_cast<int32>(EPLAYER_PATTERN::MOVING)])
		return;

	if (EMonsterState::SKILL == Get_MonsterState())
		return;

	if (FollowTime <= ShortPressThreshold) //클릭으로 인식하겠다
	{
		// Character
		if(nullptr == TargetActor)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
		else // 타겟이 존재하면 타겟을 향해 간다
		{
			if(ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
				UAIBlueprintHelperLibrary::SimpleMoveToActor(this, TargetActor);
		}

		// Actor
		if (nullptr == Highlight_BulidActor)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
		else // 타겟이 존재하면 타겟을 향해 간다
		{
			if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
			{
				UAIBlueprintHelperLibrary::SimpleMoveToActor(this, Highlight_BulidActor);

				// 해당 건물을 눌렀다면, 
				if(nullptr != Highlight_BulidActor)
				{	
					CurrentPlayerState->TargetObject = Highlight_BulidActor;
				}
			}
		}
	}
	FollowTime = 0.f;
}

// 플레이어 상태를 알림
EMonsterState AR1PlayerController::Get_MonsterState()
{
//	if (nullptr != R1Player)
//		return R1Player->MonsterState;

	return EMonsterState::END;
}

// 플레이어의 상태를 저장함
void AR1PlayerController::Set_MonsterState(EMonsterState _monsterState)
{
//	if (R1Player)
//		R1Player->MonsterState = _monsterState;
}

void AR1PlayerController::TickCursorTrace()
{
	FHitResult OutCursorHit;

	if (bMousePressed)
		return;

	if (false == GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, OUT OutCursorHit)) // 충돌 관련 부분
		return;
	
	// 어떤 Actor와 충돌했는가?
	AInteractive_Buildings* LocalBuildingActor = Cast < AInteractive_Buildings>(OutCursorHit.GetActor());
	AR1Character* LocalHighlightActor = Cast<AR1Character>(OutCursorHit.GetActor());
	AEnvironment_Floor* LocalFloorActor = Cast<AEnvironment_Floor>(OutCursorHit.GetActor());

	if (ESCENE::INGAME == CurrentPlayerState->Player_CurrentScene)
	{
		TraceCharacterType(LocalHighlightActor);
		TraceFloorType(LocalFloorActor);
		TraceActorType(LocalBuildingActor);
	}
}

void AR1PlayerController::TraceCharacterType(AR1Character* LocalHighlightActor)
{
	if (nullptr == LocalHighlightActor)
	{
		// 주시하고 있지 않을 때 : 하이라이트를 없앤다.
		if (HighlightActor)
		{
			if (ESCENE::LOBBY != CurrentPlayerState->Player_CurrentScene)
			{
				HighlightActor->UnHighlight();
				HighlightActor = LocalHighlightActor;
			}
		}
	}
	else if (nullptr != LocalHighlightActor)
	{
		// 주시하고 있을 때 : 하이라이트를 켜 준다.
		if (HighlightActor)
		{
			// 이미 주시한 상태에서 다른 애일 때
			if (HighlightActor != LocalHighlightActor)
			{
				if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
				{
					HighlightActor->UnHighlight();
					AR1Player* TestPlayer = Cast<AR1Player>(LocalHighlightActor);
					if(TestPlayer)
					{
						TestPlayer->Highlight();
						HighlightActor = LocalHighlightActor;
					}
				}
				else
				{
					HighlightActor->UnHighlight();
					if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
					{
						AR1Player* TestPlayer = Cast<AR1Player>(LocalHighlightActor);
						if (TestPlayer)
							TestPlayer->Highlight();
					}
					else
						LocalHighlightActor->Highlight();
					HighlightActor = LocalHighlightActor;
				}
			}
		}
		else
		{
			// 새로운 타깃
			if (ESCENE::LOBBY == CurrentPlayerState->Player_CurrentScene)
			{
				AR1Player* TestPlayer = Cast<AR1Player>(LocalHighlightActor);
				if (TestPlayer)
					TestPlayer->Highlight();
			}
			else
				LocalHighlightActor->Highlight();
			HighlightActor = LocalHighlightActor;
		}
	}

}


void AR1PlayerController::TraceFloorType(AEnvironment_Floor* LocalFloor)
{
	if (nullptr == LocalFloor)
	{
		if (Highlight_FloorActor)
		{
			Highlight_FloorActor->UnHighlight();
		}
	}
	else if (nullptr != LocalFloor)
	{
		if (Highlight_FloorActor)
		{
			if (Highlight_FloorActor != LocalFloor)
			{
				Highlight_FloorActor->UnHighlight();
				LocalFloor->Highlight();
			}
		}
		else
		{
			LocalFloor->Highlight();
		}
	}
	Highlight_FloorActor = LocalFloor;
}

void AR1PlayerController::TraceActorType(AInteractive_Buildings* LocalBuildingActor)
{
	if (nullptr == LocalBuildingActor)
	{
		if (Highlight_BulidActor)
		{
			Highlight_BulidActor->UnHighlight();
		}
	}
	else if (nullptr != LocalBuildingActor)
	{
		if (Highlight_BulidActor)
		{
			if (Highlight_BulidActor != LocalBuildingActor)
			{
				Highlight_BulidActor->UnHighlight();
				LocalBuildingActor->Highlight();
			}
		}
		else
		{
			LocalBuildingActor->Highlight();
		}
	}
	Highlight_BulidActor = LocalBuildingActor;
}

void AR1PlayerController::ChaseTargetAndAttack(AR1Character* LocalHighlightActor)
{
	// Player 차례일 때만 고를 수 있도록 한다.
	if(EFIGHT_AUTHORITY::PLAYER == R1Player->Get_FightAuthority())
	{
		if (nullptr != LocalHighlightActor)
		{
			if (HighlightActor)
			{
				if (HighlightActor != LocalHighlightActor)
				{
					HighlightActor->UnHighlight();
					AR1Monster* SelectMonster = Cast<AR1Monster>(LocalHighlightActor);
					if (SelectMonster)
					{
						SelectMonster->Highlight();
						HighlightActor = LocalHighlightActor;
					}
				}
			}
			else
			{
				AR1Monster* SelectMonster = Cast<AR1Monster>(LocalHighlightActor);
				if (SelectMonster)
					SelectMonster->Highlight();

				HighlightActor = LocalHighlightActor;
			}
			// Target Monster를 Select Monster로 변경
			R1Player->Set_SelectTargetMonster(Cast<AR1Monster>(HighlightActor));
		}
	}
}

void AR1PlayerController::ChaseTargetAndInteractive()
{
	if (nullptr == CurrentPlayerState || nullptr == CurrentPlayerState->TargetObject)
		return;
}

void AR1PlayerController::ChaseLobbyInteractive(AR1Character* LocalHighlightActor)
{
	if (nullptr != LocalHighlightActor)
	{
		if (HighlightActor)
		{
			if (HighlightActor != LocalHighlightActor)
			{
				HighlightActor->UnHighlight();
				if (AR1Player* SelectPlayer = Cast<AR1Player>(LocalHighlightActor))
				{
					HighlightActor = SelectPlayer;
					SelectPlayer->Highlight();
				}
			}
		}
		else
		{
			AR1Player* SelectPlayer = Cast<AR1Player>(LocalHighlightActor);
			if (SelectPlayer)
			{
				SelectPlayer->Highlight();
			}
			HighlightActor = LocalHighlightActor;
		}
	}
}

