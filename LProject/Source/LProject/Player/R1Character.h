#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/R1Highlight_Interface.h"
#include "Define/R1Define.h"
#include "GamePlayTagContainer.h"
#include "R1Character.generated.h"

enum class EIMPACT
{
	IMPACT_ATTACK,
	IMPACT_END
};

UCLASS()
class LPROJECT_API AR1Character : public ACharacter, public IR1Highlight_Interface
{
	GENERATED_BODY()

public:
	AR1Character();

protected:
	virtual void BeginPlay() override;
	
	// Fight : 치고 빠지는 함수
	bool HitAndDash(AR1Character* _thisObj, FVector _targetPos, bool _endTiming, float _timing = 0.5f);
	// 코인 퍼트리기
	bool Drop_Coin(FVector _target,bool _bFollowCoin = false, FVector _followTarget = FVector::ZeroVector);

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void HandleGameplayEvent(FGameplayTag EventTag);
	bool DecreaseHP(float& _currentHP, float _targetHP, float DeltaTime, float _speed = 3.f);

	UTexture2D* Get_ProfileTexture() { return Profile_Texture; }

public :
	FVector		Get_InitPos() { return InitPos; }
	/* Target Monster 관련*/
	virtual void Highlight() override;
	virtual void UnHighlight() override;

	/* 체력 관련*/
	virtual void OnDamged(int32 _damage, TObjectPtr<AR1Character> _Attacker);
	void OnHill(int32 _damage, TObjectPtr<AR1Character> _Attacker);
	virtual void OnDead(TObjectPtr<AR1Character> _Attacker);

	/* 능력치*/
	void Set_AbilityValue(ABILITY_TYPE _ability, float _value);
	float Get_AbilityValue(ABILITY_TYPE _ability) const;
	bool CalculateAbility(ABILITY_TYPE _ability);

	bool						Get_DashAndHitStart() { return bHitAndDashStart; }

protected :
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = { false };

	bool HPAmount = { false };

	// Monster와 싸울 시, 회피 특성을 위해 플레이어 이전 위치를 고정시켜줄 변수
	FVector InitPos;

	// 애니메이션에게 전달할 대시 호출
	UPROPERTY()
	bool			bHitAndDashStart = { false };

	UPROPERTY(BlueprintReadOnly)
	bool bDead = { false };

	/* 대쉬에 필요한 변수 */
	UPROPERTY()
	float DashAttack_Timer = { 0.f };

	UPROPERTY()
	bool bCompletedDash = { false };
	
	EIMPACT IsImpact = { EIMPACT::IMPACT_END };

	// :: 턴 교체
	// 다음 턴 대기
	UPROPERTY()
	float NextTurn_ReadyTimer = { 0.f };
	
	// 파티클을 사용하는 시간
	UPROPERTY()
	float Particle_Timer = { 0.f };

	// 모든 액션이 끝나고 대기만을 남긴 상태
	UPROPERTY()
	bool bNextTurn = { false };

	// MaxpHp Update 시 필요한 함수
	UPROPERTY()	
	bool bSaveTargetHp = { false }; // 목표 maxhp를 저장
	UPROPERTY()
	bool bSaveTargetAttack = { false }; // 목표 maxAttack를 저장
	UPROPERTY()
	bool bSaveTargetHill = { false }; // 목표 maxHill를 저장

	UPROPERTY()
	bool bSaveMaxPlus = { false }; // 음수인지 양수인지 판별

public :
	UPROPERTY(BlueprintReadWrite)
	EMonsterState MonsterState = EMonsterState::END;

	// Monster 프로필
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile Texture")
	UTexture2D* Profile_Texture;

	// :: 캐릭터의 정보
public : 
	/* ====== HP ======= */
	// 예비 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Hp					= 100; 
	// 현재 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHp		= 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentLevel		= { 1 };

	// 목표 MaxHp
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetMaxHp = 100;
	// Max HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp			= 100;

	/* ====== Attack ======= */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetAttack = 30; // 변경해야 할 값

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentAttack = 30;


	/* ====== Ability ======= */
	// 캐릭터의 능력치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> AbilityValues;
	
	// 자가 힐 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FinalHill			= 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetHill			= 15;
};
