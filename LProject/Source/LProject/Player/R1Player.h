#pragma once

#include "CoreMinimal.h"
#include "R1LoadPlayer.h"
#include "Item/R1ItemData.h"
#include "RunTime/Engine/Public/EngineUtils.h"
#include "R1Player.generated.h"

UCLASS(Abstract)
class LPROJECT_API AR1Player : public AR1LoadPlayer
{
	GENERATED_BODY()

public:
	AR1Player();
	virtual void Tick(float DeltaTime) override;

protected : 
	virtual void BeginPlay() override;
	virtual void Pattern_Function() PURE_VIRTUAL(AR1Player::Pattern_Function, );
	void Impact_MonsterPattern();



 public: //============= For. Find 함수 =============
	template <typename T>
	static TArray<T*> FindActorsByClass(UObject* WorldContextObject);

	template <typename T>
	static TArray<UUserWidget*> FindWidgetsByClass(UObject* WorldContextObject);



public: //============= For. Getter Setter  =============
	// ----------------------------- GETSET. LOBBY ----------------------------- 
	//  1. Material 관련 함수
	void								Player_MaterialCustomize(int32 _materialCnt, _int32 _customCnt, class UPlayerSetting_Widget* _playerbarWbp = nullptr);
	void								Load_PlayerMaterial(CUSTOMIZE_PLAYER _customType, class UMaterialInterface* _material);


	// ----------------------------- GETSET. INGAME -----------------------------  
	// 게임의 첫 시작을 맞이하였는가?
	bool								Get_GamePlayStart();
	// 다음 플레이어로 바꿀 것인가?
	bool								Change_Player();
	//  Jump 할 때 파티클 스폰
	void								Spawn_JumpParticle();

	//  아이템 사용
	bool								Using_Item(EITEM_TYPE _item);
	//  Player 에너지를 전부 채울 것인가?
	void								Full_Energy(bool _full);
	
	// 바닥 관련
	class AEnvironment_Floor*	Get_PrevFloor() { return Prev_Floor; }
	void								Set_PrevFloor(class AEnvironment_Floor* _floor) { Prev_Floor = _floor; }

	// Mouse Control이 가능함을 알림
	bool								Get_MouseMoving();
	void								Set_StopMouseMoving();
	void								Add_InventoryItem(FItemData_Asset* _asset);
	
	// 만약 떨어졌다면,
	bool								Falling_Player();
	void								Falling_Reset();

	// 회피 하였는가?
	bool								Get_ShirkJump() { return bIsShirkJumping; }
	
	// Next Turn
	void								Set_NextTurnStart(bool _start = true);
	void								Set_ChaosMove_AndNextTurnStart(bool _start = true);
	// 턴이 끝남
	bool								End_Turn(bool _bneedzoom);

	// Inventory bOpeen
	void								Set_OpenInventory(bool _open) { bOpenInven = _open;  }
	bool								Get_OpenInventory() { return bOpenInven; }

	// Switching Camera (카메라가 돌아갈 때 상호작용을 막기 위해서)
	bool								IsInteractable();
	// 마지막에 접촉한 Floor 전송
	void								Set_PlayerContextLastFloor(class AEnvironment_Floor* _floor) { PlayerContext_LastFloor = _floor; }

	// 최대 Hp 업데이트
	bool Update_MaxHp(int _value, bool _directly = false);
	// Attack 업데이트
	bool Update_MaxAttack(int _value);
	// Hill 업데이트
	bool Update_MaxHill(int _value);

	// Set Text
	void Set_HpText(float _maxHp, float _hp);
	void Set_AttackText(float _attack);
	void Set_HillText(float _hill);


	// ----------------------------- GETSET. FIGHT SCENE -----------------------------  
	// 권한 관련 함수
	bool								IsNearbyAttack() { return bNearbyAttack;  }
	void								Set_FightAuthority(EFIGHT_AUTHORITY _type);
	EFIGHT_AUTHORITY			Get_FightAuthority();

	// 트랩 관련
	void								OnTrapStep();

	// 행동 관련
	void								Set_ShirkEnd(bool _end = true) { bShirkEnd = _end; }
	bool								Get_Jumping() { return bIsJumping;  }

	// 에너지 관련
	bool								Use_PlayerEnergy(bool _using);
	bool								Add_PlayerEnergy(int32 _add);
	bool								Get_ShowClassInfo();

	// 공격할 몬스터 선택
	void								Set_SelectTargetMonster(class AR1Monster* _monster);

	// 싸움 끝남
	void								Set_EndFight(bool _end) { bFightEnd = _end; }
	bool								Get_EndFight() { return bFightEnd;  }
	
	// 타이밍에 맞춰서 몬스터 공격
	void								Set_RidAllMonster() { bRidAllMonster = true; }
	void								Set_TimingAttack() { bAttackStart = true;  }
	bool								Get_TimingAttack() { return bAttackStart; }
	// 타이밍에 맞춰 공격하는 것을 끝냄
	void								Set_EndTimingAttack() { bAttackEnd = true; }
	bool								Get_EndTimingAttack() { return bAttackEnd;  }
	
	// 몬스터가 에너지 확률 (MonsterEnergy_Widget) 을 시작할 수 있도록 하는 함수
	void								Next_Energy();


	// ----------------------------- GETSET. BOSS MAP -----------------------------  
	FVector							Get_PlayerCameraCompPos();
	// BossMap일 때 플레이어를 체인지한다.
	void								Set_BossMap_PlayerChange() { bBossMap_PlayerChange = true; }


	// ----------------------------- GETSET. OTHER -----------------------------  
	// Level 변경
	void								Change_Level(ESCENE _scene);
	// 에셋 로드
	void								Load_Assets();

	// Hp 프로그래스 바 조정 이 전에 TargetHp 값을 변동시킬 무언가 (EX. OnDamaged)가 필요하다.
	void								DecreaseHP(float& _currentHP, float _targetHP, float DeltaTime, float _speed = 3);



protected : //============= For. 행동 함수  =============
	// Shirk
	bool							ShirkJump();
	void							ShirkJumpStop();

	// Attack Jump : 트랙에 걸렸을 때 맞을 때 날라가기
	void							HitJump();
	void							HitJumpStop();

	// 뒤로 밀리기
	void							Start_PullBack();
	bool							PullBack();
	bool							Return_InitPos();

	// 에너지 사용
	void							Use_SkillEnergy(bool _use);

	// 3D Widget을 호출하는 함수
	void							Call_StateWidget(FText _text);


private:  //============= For. 델리게이트  =============
	UFUNCTION()
	void		OnBeginOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void		CameraSetting();


private :  //============= For. 세팅 함수  =============
	void		Setting_LobbyScene();
	void		Setting_IngameScene();
	void		Setting_InFightScene();
	bool		Setting_PlayerState();

	bool		Setting_Loading();
	bool		Setting_TickLoading();

protected : //============= For. FIGHT SCENE 관련 함수  =============
	// 싸울 몬스터 생성
	bool		Create_FightMonster();
	bool		Attack();
	bool		StopAttack(EPLAYER_PATTERN _ePattern);
	bool		TimingAttack();
	void		Stop_TimingAttack();

	virtual void Landed(const FHitResult& Hit) override;

	bool		Hit();
	bool		Hill();
	
	bool		HitAndDash(AR1Character* _thisObj, FVector _targetPos, bool _endTiming = false);
	void		Stop_HitAndDash();
	

	bool		Ready_NextTurn(EPLAYER_PATTERN _pattern, EFIGHT_AUTHORITY _nextTarget);
	void		Player_Dead();

public : //============= For. Default 변수  =============
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor>			SceneCamera;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent>		m_SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent>			m_Camera;


protected : //============= For. 무기 변수  =============
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AR1Weapon> Player_Weapon;

    UFUNCTION(BlueprintCallable, Category="Weapons")
    const TArray<AR1Weapon*>& GetWeaponArray() const { return GetWeapon_Array; }

	UPROPERTY()
	TArray<AR1Weapon*> GetWeapon_Array;

	UClass* Find_Weapon = { nullptr };


protected : //============= For. Widget 변수  =============
	// 0 : 3D Widget
	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UWidgetComponent* CurrentState_Widget;
	UPROPERTY()
	TObjectPtr<class UUserWidget> WBPCurrent_Instance = { nullptr };

	// 1 : InFight
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> Skill_Widget;
	UPROPERTY()
	class USelectSkill_Widget*			SkillWidget_Instance = { nullptr };

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> ResultFight_Widget;
	UPROPERTY()
	class UResultFight_Widget*			ResultFight_Instance = { nullptr };

	// 2 : In game
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerBar_Widget;
	UPROPERTY()
	class UPlayerBar_Widget*			PlayerBar_Instance = { nullptr };

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerHP_Widget;
	UPROPERTY()
	class UPlayerHP_Widget*			PlayerHP_Instance = { nullptr };
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> Mission_Widget;
	UPROPERTY()
	class UMission_Widget*				 Mission_Instance = { nullptr };
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> Second_Widget;
	UPROPERTY()
	class USecond_Widget*				Second_Instance = { nullptr };
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> Inventory_Widget;
	UPROPERTY()
	class UPlayerInventory_Widget*	Inventory_Instance= { nullptr };

	//  3 : Lobby
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerSetting_Widget;
	UPROPERTY()
	class UPlayerSetting_Widget* PlayerSetting_Instance = { nullptr };


protected : // ============= For. Behavior Variable =============
	// 1. Falling
	UPROPERTY()
	bool bFalling = { false };
	UPROPERTY()
	float FallingTimer = { 0.f };
	UPROPERTY() // 플레이어와 접촉한 마지막 floor
	TObjectPtr<AEnvironment_Floor> PlayerContext_LastFloor = { nullptr };

	//  2. Shirk
	UPROPERTY()
	bool bIsJumping = { false };
	UPROPERTY()
	bool bIsShirkJumping = { false };
	UPROPERTY()
	bool bShirkEnd = { false };
	UPROPERTY()
	FVector Fight_ShirkPosition; // ( Fight )

	// 3. Hit And Dash
	UPROPERTY()
	bool		bHitAndDash	 = { false };
	UPROPERTY()
	float		HitAndDash_Timer = { 0.0f };

	// 4. Attack
	/* Timing Attack : 타이밍 맞춰서 때리기 */
	UPROPERTY()
	bool		bAttackStart = { false };

	UPROPERTY()
	bool		bAttackEnd = { false };

	UPROPERTY()
	bool		bFightEnd = { false };

	UPROPERTY()
	bool		bRidAllMonster = { false }; // 모든 몬스터를 처치하였는가?

	UPROPERTY()
	bool		bNearbyAttack = { false };// 가까이에 다가가 공격하겠는가?

	// 5. Inventory
	UPROPERTY()
	bool			bOpenInven = { false };

	// 6. 3D Widget
	UPROPERTY()
	float			StateWidget_Timer = { 0.f };	 // Widget의 움직임을 제한
	UPROPERTY()
	FVector2D	Origin_StateWidget = {}; // Widget의 초기 위치 저장

	UPROPERTY()
	float fDWOrigin{};

	UPROPERTY()
	bool bUsingItem = { false };

	// 7. 뒤로 밀었을 때 밀리게 하는 함수
	FVector		PullBackPos;
	bool			bPullBack_Start = { false };
	bool			bPullBack = { false };
	bool			bPullBack_End = { false };
	float			PullBack_Timer = { 0.f };

	protected:  /* ========= For. Boss Scene ============= */
		bool			bBossMap_PlayerChange = { false };

protected :
	TObjectPtr<class AEnvironment_Floor> Prev_Floor;


protected :
	UPROPERTY()
	class UParticleSystem* Jump_Particle; 

	UPROPERTY()
	class UParticleSystem* AimAttack_Particle;


protected :  // Lobby
	CUSTOMIZE_PLAYER						PlayerMaterial_Cnt[static_cast<int32>(CUSTOMIZE_PLAYER::END_CUSTOM)];
	

};

