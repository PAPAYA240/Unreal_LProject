#pragma once

#include "CoreMinimal.h"
#include "Player/R1Character.h"
#include "Item/R1ItemData.h"
#include "Define/R1Define.h"
#include "R1Monster.generated.h"


UCLASS(Abstract)
class LPROJECT_API AR1Monster : public AR1Character
{
	GENERATED_BODY()
	
public :
	AR1Monster();
	virtual void				Tick(float DeltaTime) override;

protected:
	virtual void				BeginPlay() override;
	virtual bool				Pattern_Function();
	virtual void				impact_PlayerPattern() PURE_VIRTUAL(AR1Monster::impact_PlayerPattern, );

	void						Operate_FightScene();
	
	void						Setting_MonsterAbility(int32 _level);

public : /* For. Getter Setter*/
	/* Look Rotation() */
	void						Set_LookatPlayer(bool _look)		{ bLookAtRotation = _look;  }
	bool						Get_LookatPlayer()						{ return bLookAtRotation;  }

	void						Set_EnergyStart(bool _start);

	/* Monster Information */
	void						Set_MonsterNumber(int32 _num) { MonsterNumber = _num; }
	int32						Get_MonsterNumber()				{ return MonsterNumber;  }

	EMONSTER_TYPE		Get_MonsterType()					{ return MonsterType; }
	bool*						Get_Monster_Pattern()				{ return Monster_Pattern; }
	FItemData_Asset*		Get_RewardItem();

	void						Set_FindConnectMonster()			{ bFindConnectMonster = true; }
	void						Set_TimingAttack()						{ bAttackStart = true; }

	void						Get_UseEnergy(bool _using); 

	FText						Get_MonsterName()					{ return MonsterName; }
	FText						Get_MonsterDocument()				{ return MonsterDocument;  }

	EREGION_TYPE			Get_Monster_RegionType()			{ return Monster_RegionType;  }

	void						Set_FightActiveStop(bool _active) { bFightActityStop = _active; }

protected :
	// 자체 발광 : HIT
	void						Start_Emissive(EMONSTER_PATTERN _pattern);
	void						Change_Emissive();

	bool						IsMonsterTurn();
	void						OnBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// For. Attack
	bool						Attack_Type(); // Monster 차례에 할 행동 패턴을 정해준다.
	void						Attack_Stop();

	// For. HP
	void						Set_ProgressBar(float _hp);
	void						DecreaseHP(float& _currentHP, float _targetHP, float DeltaTime, float _speed = 5);

	bool						 Hit();
	bool						 Hill();

	// For. Particle
	/* 파티클 시스템 작동*/
	bool ParticleStart(UParticleSystem* _particle, bool _static, float _timer = 0.f, FVector _Location = FVector{}, int _particleCnt = 0);
	/* 파티클 스폰*/
	bool SpawnParticle(UParticleSystem* _particle, FVector _Location, bool _static, float _timer, int32 _particleCnt = 0);
	/* 파티클 종료 */
	void DestroyParticle(UParticleSystem* _particle);
	/* 파티클이 다수일 때 움직임 */
	bool Move_ParticleArray(FVector _Location, int32 _particleCnt);

	// For. Function related to fight turn
	/* 해제시킬 패턴과, 다음 턴의 인물 : 싸울 때 다음 턴으로 넘어갈 수 있도록 한다 */
	bool Ready_NextTurn(EMONSTER_PATTERN _pattern, EFIGHT_AUTHORITY _nextTarget, float _delayTimer = 0.f);
	/* 치고 빠질 때 사용할 함수*/
	bool HitAndDash(AR1Character* _thisObj, FVector _targetPos, float _timing = 0.25f);
	void Stop_HitAndDash();

	// For. Boss Scene
	bool IsActivity() { return bActivity;  }

private:
	/* 플레이어를 바라볼 수 있도록 함 */
	void							Lookat_Player(); 
	/* Fight Scene : 몬스터 스폰 위치 설정 */
	void							MonsterSpawn();
	/* 몬스터가 죽었는가? */
	void							Monster_Dead();

private: /* For. BeginPlay Setting */
	bool							Find_Player();
	bool							Find_Widget();
	bool							Setting_Item();
	bool							Setting_Animation();
	bool							Setting_Material();

protected : // ======== For.Widget =========
	/* 1. 보상 획득 아이템 Widget */
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> RewardItem_Widget;
	UPROPERTY()
	class URewardItem_Widget*		RewardItem_Instance = { nullptr };

	/* 2. 몬스터 정보 Widget */
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget>		MonsterInfo_Widget;

	// 3. 몬스터와 싸울 준비를 한다.
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UReadyFight_Widget*			ReadyFightWidget;

	// 4. 몬스터 HP Widget
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget>		MonsterHP_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UMonsterHP_Widget*			MonsterHPBar;

	// 5. 몬스터 공격 에너지 Widget
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget>		MonsterEnergy_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UMonsterEnergy_Widget*	MonsterEnergy_Instance;
	 
	// 6. 3D State Widget
	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UWidgetComponent* CurrentState_Widget;
	UPROPERTY()
	TObjectPtr<class UUserWidget> WBPCurrent_Instance = { nullptr };

protected : // For.Player Look Rotation
	// 플레이어를 향해 돌아봤는가?
	UPROPERTY()
	bool			bLookAtRotation = { false }; 
	// Rotation Speed
	UPROPERTY()
	float			RotationSpeed = 10.f;	
	// 기존 Rotation
	UPROPERTY()
	FRotator		 Init_Rotation ={}; 
	// Monster를 찾을 때 주변에 있는 몬스터 확인
	bool			bFindConnectMonster = { false };

protected :  // For.Fight
	// 몬스터 스폰 여부
	UPROPERTY()
	bool			bFight_MonsterSpawn = { false }; 
	// 몬스터가 스폰될 때까지의 여유 시간
	UPROPERTY()
	float			Fight_MonsterSpawnTimer	= { 0.f }; 
	UPROPERTY()
	float			MonsterAttack_Timer	= { 0.f }; 
	// 치고 빠질 때 사용할 변수
	UPROPERTY()
	bool			bHitAndDash = { false };

	UPROPERTY()
	float			HitAndDash_Timer	 = { 0.0f };
	// Hidden Attack과 Default Attack을 선택해 줄 변수
	UPROPERTY()
	bool			bSelect_AttackType = { false };
	// Attack 시, 타이밍을 맞추고 싶을 때 
	UPROPERTY()
	bool			bAttackStart = { false };
	UPROPERTY()
	FVector		Origin_Scaled = {};
	UPROPERTY()
	FVector		Target_Scaled = {};
	UPROPERTY()
	bool			bFightActityStop = { false }; // Dead와는 다른, true 시 해당 Monster의 턴이 넘어간다
	UPROPERTY()
	bool			bFightEnd			= { false }; // Fight가 완전이 끝났다면 타이밍에 맞춰 Reward Widget을 호출한다.
	UPROPERTY()
	bool			bLastMonsterCheck = { false };
	UPROPERTY()
	float RewardWidget_Timer = { 0.f };
protected : /* For.Boss Scene*/
	UPROPERTY()
	bool			bActivity = { false }; // 활동이 가능한가?

protected: /* For. Player or Monster Information */
	// :: 플레이어
	UPROPERTY()
	TObjectPtr<class AR1Player>			R1Player	= { nullptr };
	// "현재" 플레이어 정보
	UPROPERTY()
	TObjectPtr<class AMyPlayerState>	CurrentPlayerState	 = { nullptr };
	// 몬스터 담당 지역
	UPROPERTY()
	EREGION_TYPE								Monster_RegionType = { EREGION_TYPE::END_TYPE };
	// :: 몬스터 타입
	UPROPERTY()
	EMONSTER_TYPE							MonsterType	= { EMONSTER_TYPE::END };
	// 몬스터에게 번호를 매겨 필드에서 삭제 여부를 판단할 것임
	UPROPERTY()
	int32 MonsterNumber = { 0 };
	// 몬스터 이름
	UPROPERTY()
	FText											MonsterName;
	// 몬스터 설명
	UPROPERTY()
	FText											MonsterDocument;

	// 몬스터 패턴 정보
	bool											Monster_Pattern[static_cast<int32>(EMONSTER_PATTERN::END)] = {};
	// Monster가 배출할 수 있는 아이템 종류
	TArray<FItemData_Asset*>				ItemAssets;

protected : /*For.Weapon*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent*				Weapon;

protected : /* For. Particle */
	// 현재 진행 중인 파티클
	UPROPERTY()
	TObjectPtr<class UParticleSystemComponent>	ParticleComponent = { nullptr };
	// 다수의 파티클이 필요할 때 사용
	UPROPERTY()
	TArray<class UParticleSystemComponent*>		ParticleArray;

	// 이펙트 모음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem>							Hill_Particle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem>							Attack_Particle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem>							Hidden_Particle;
	UPROPERTY()
	TObjectPtr<UParticleSystem>							Appearance_Particle;


	// 파티클 출력을 시작할 것인가?
	UPROPERTY()
	bool															bParticleStart	= { false };
	// 출력할 파티클 개수
	UPROPERTY()
	int																ParticleCnt	= { 0 };

protected: // 발광
	class UMaterialInstanceDynamic* DynamicMaterialInstance;
	UPROPERTY()
	bool			bIncreasing		= { true };
	UPROPERTY()
	float			CurrentValue	= { 0.f };
	UPROPERTY()
	bool			bEmissive		= { false };
	UPROPERTY()
	bool			bEmissiveStart = { false };
	BYTE			EmissivePattern = {};

};
