// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Define/R1Define.h"
#include "GameplayTagContainer.h"
#include "R1PlayerController.generated.h"

struct FInputActionValue;

UCLASS()
class LPROJECT_API AR1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AR1PlayerController(const FObjectInitializer& ObjectInitializer);

	void BackRotation_Jump();
	class AR1Character* Highlight_Character() { return HighlightActor;  }
	void Set_Highlight_Character(class AR1Character* _character) { HighlightActor = _character; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	bool Setting_Player();

public :
	virtual void HandleGameplayEvent(FGameplayTag EventTag);

	/* For. function */
private: // 1. Action
	void					Input_JumpAction(const FInputActionValue& InputValue);
	void					Input_ZoomAction(const FInputActionValue& InputValue);
	void					Input_KeyBoardMovingAction(const FInputActionValue& InputValue);
	void					Input_EnergyAction(const FInputActionValue& InputValue);
	void					Input_InventoryAction(const FInputActionValue& InputValue);

private : /* 2. Cursor */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	
	EMonsterState Get_MonsterState();
	void Set_MonsterState(EMonsterState _monsterState);

private :
	void TickCursorTrace(); /* Monster 외곽선 처리*/
	void TraceCharacterType(class AR1Character* LocalHighlightActor);
	void TraceFloorType(class AEnvironment_Floor* LocalFloor);
	void TraceActorType(class AInteractive_Buildings* LocalBuildingActor);

	void ChaseTargetAndAttack(class AR1Character* LocalHighlightActor); // 일정 간격 이상 공격 처리
	void ChaseTargetAndInteractive();
	void ChaseLobbyInteractive(class AR1Character* LocalHighlightActor);

	/* For. Variable*/
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAnimMontage> AttackMontage;
		
	// Zoom
    UPROPERTY(EditAnywhere)
    float ZoomSpeed = 20.0f;

    UPROPERTY(EditAnywhere)
    float MinFOV = 30.0f;

    UPROPERTY(EditAnywhere)
    float MaxFOV = 100.0f;

public : // Cursor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold = 0.3f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UNiagaraSystem> FXCursor; // 커서 이펙트

private:
	FVector CachedDestination; /* 목적지를 기억하기 위함 */
	float FollowTime;
	bool bMousePressed = false; /* 마우스를 누르고 있는가?*/

protected :
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AR1Character>				TargetActor;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AR1Character>				HighlightActor;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AEnvironment_Floor>	Highlight_FloorActor;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AInteractive_Buildings>	Highlight_BulidActor;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AR1Player> R1Player;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AMyPlayerState>				CurrentPlayerState = { nullptr };

	FVector BackwardDirection;
	bool b_One_Rotation = { false };
	bool bPrevJump = { false };
};