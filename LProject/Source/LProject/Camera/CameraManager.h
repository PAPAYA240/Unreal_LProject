
#pragma once

#include "CoreMinimal.h"
#include "Define/R1Define.h"
#include "Camera/PlayerCameraManager.h"
#include "CameraManager.generated.h"

struct PlayerCombatCameraSpot
{
	FVector TargetLocation = FVector(-180, -330, 320.f);
	FRotator TargetRotation = FRotator(-28.7f, 129.7f, 1.6f);/* Y,Z,X 순서*/
};

enum FOCUS_CAMERA 
{
	FOCUS_PLAYER,
	FOCUS_MONSTER,
	FOCUS_OBJECT,
	FOCUS_END
};

UCLASS()
class LPROJECT_API ACameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public :
	ACameraManager();

protected:
	virtual void UpdateCamera(float DeltaTime) override;
	virtual void BeginPlay() override;
	bool			Find_Camera();

public: //============= For. Getter Setter =============
	// ----------------------- 1. GetSet. LOBBY -----------------------  
	class UCameraComponent* Get_CameraActor() {
		return CameraActor;
	}
	void Set_CameraActor(class UCameraComponent* _compon) { CameraActor = _compon; }

	// ----------------------- 2. GetSet. FIGHT -----------------------  
	bool IsFight() { return bFightStart; }
	bool IsFightCamerChange() { return bCameraChange;  }
	// 때릴 때 줌 확대를 위하여
	void InitCamera_NextTurn() {
		fCameraChange_Timer = 0.f;
		bCameraChange = false;
	};
	void Set_HitZoom(bool _zoom) { bHitZoom = _zoom; } 

	bool CameraZoomIn(float _zoomTime);
	bool CameraZoomOut(float _distance);
	bool Initialize_CameraPos();

	bool Get_Switching_Camera() { return bCameraSwitching; }
	bool PitchRotation_Camera(float _timer, float _speed);

	// ----------------------- 3. GetSet. INGAME -----------------------  
	// // 인게임 플레이(게임의 첫 시작)를 시작했는가?
	bool Get_GamePlayStart() { return bStartGamePlay;  }
	bool EnterBossMap_Camera();
	void Set_PrevCamPos(FVector _pos) { PrevCamPos = _pos; }
	FVector Get_PrevCamPos() { return PrevCamPos; }
	
	void PlayerBack_CamPos(float _distance);
	void Set_OperateTelescope(bool _operate) { bOperateTelescope = _operate;  }

	// ----------------------- 4. GetSet. BOSS MAP -----------------------  
	// First Cut Scene
	bool Get_CutSceneAnimStart() { return CutSceneAnimStart; }
	
	//Second Cut Scene
	void Set_BCCameraIndex(EBOSSMAP_CAMERA _idx) { BCamera_Index = _idx; }
	EBOSSMAP_CAMERA Get_BCCameraIndex() { return BCamera_Index; }
	int32 Get_BCSecondCutScene_CameraIndex() { return BCSceneCameraIndex;  }// 2번 째 컷 씬이 돌 때 진행되고 있는 포지션

	// 플레이어의 보스 공격 [Aim Attack] 이 끝났는가?
	bool	Get_AimAttackEnd() { return bAimAttackEnd;  }


private :  //============= For. 기능 함수 =============
	void Opreate_Camera(float DeltaTime);
	bool Change_CameraTime(float _speed, float _dist, float _time = 0.f);


private : //============= For. SCENE 함수 =============
	// ----------------------- 1. Func. LOBBY -----------------------  
	void Lobby_Camera(float DeltaTime);


	// ----------------------- 2. Func. INGAME -----------------------  
	 // 기본 카메라 무빙
	void Camera_Move(float DeltaTime);
	// 포커스를 어디에 맞출 것인가?
	bool FocusOnObject(FOCUS_CAMERA _actor, float DeltaTime); 
	// 오브젝트에 초점을 맞추기 시작했는가?
	bool Camera_Focus();
	// 플레이어가 아직 오브젝트에 볼 일이 있는가?
	bool CheckObject() ;
	// 첫 게임을 시작할 때 
	void GamePlayStart(); 
	// 카메라 흔들기
	void ShakeCamera();
	

	// ----------------------- 3. Func. FIGHT -----------------------  
	void Fight_InitCamera(float DeltaTime);
	void Fight_SwitchOrderCamera(float DeltaTime);
	void Fight_CameraPosition_Setting();
	// 싸움이 끝이 났을 때
	void Fight_End(float DeltaTime);
	

	FVector2D GetWindowSize();
	bool Find_Player();


	// ----------------------- 4. Func. BOSS MAP -----------------------  
	/* 1. Cam Type*/
	void BossScene_BossCam();
	void BossScene_PlayerCam();

	/* 2. Start Cam*/
	void Find_BossScene_CamPos();
	void FirstCutScene();
	bool BossAnimStart();

	/* 3. Boss Attak Type Cam*/
	// 보스 맵 게임 중일 때 카메라 전환이 필요한가?
	bool InBossMap_CameraChange(); 
	void BossMap_ChangeCamera(float DeltaTime);
	void BossMap_CameraPosition_Setting();
	void BossMoveFrom_Camera();
	// Boss Moving Type : Camera 회전 함수 
	void CameraAround(class AR1Monster* TargetMonster, float _aroundSpeed);



private: //============= For. 변수 =============
	// ----------------------- 1. Ver. Default  -----------------------  
	TObjectPtr<class AR1Player>			R1Player = { nullptr };
	TObjectPtr<class AMyPlayerState>	CurrentPlayerState = { nullptr };

	class UCameraComponent*		CameraActor;
	PlayerCombatCameraSpot		CameraChangePos;
	FVector2D							ViewportCenter;


	// ----------------------- 2. Ver.FIGHT -----------------------  
	bool bFightStart = { false };
	float CameraMove_Timer = { 0.f };
	const float InterpSpeed = 5.0f;

	float fCameraChange_Timer = { 0.f };
	float fReady_AttackTimer = { 0.f };
	bool bCameraChange = { false };

	bool bHitZoom = { false };
	/* InFight : Ending */
	float CameraYaw = 0.0f;
	float CameraDistance = 100.0f; 

	// Roation 함수
	float RotationTimer = { 0.f };
	bool bLastRotationSave = { false };
	FRotator LastRotation = {};

	// Lobby Camera
	FVector Init_CamPosition;


	// ----------------------- 3.Ver.INGAME -----------------------  
	FVector PrevCamPos;
	bool bChange_NextCamera = { false };
	// == 처음 플레이 시작했을 때 Camera 움직임에 사용 == 
	// 게임 플레이의 첫 시작
	bool bStartGamePlay					= { false };
	bool bInGame							= { false };
	float StartGameCamera_Timer		= { 0.f };

	bool bCameraFocus					= { false };
	float ZoomIn_Timer					= { 1.f };
	bool bZoomIn							= { false };
	// 카메라 돌아가고 있을 때 or 인터렉티브 하면 안 되는 중
	bool bCameraSwitching				= { false };
	// 정찰탑을 가동시켰을 때 내려가는 보스탑
	bool bOperateTelescope				= { false }; 
	

	// ----------------------- 4. Ver.BOSS MAP -----------------------  
	// 보스 방 들어가기 전
	FTimerHandle	ShakeTimerHandle;
	// 보스맵에 들어가기로 하였는가?
	bool bEnterBossMap					= { false };
	float ShakeMagnitude				= 20.0f;
	float ShakeTimer						= { 0.f };

	// 보스맵 카메라 체인지
	class UCameraComponent* BossCameraComp;
	class UCameraComponent* PlayerCameraComp;

	EBOSSMAP_CAMERA BCamera_Index = { EBOSSMAP_CAMERA::INTOWER_START_BC };
	EBOSSMAP_CAMERA BCamera_PrevIndex = { EBOSSMAP_CAMERA::END_BC };
	TArray<PlayerCombatCameraSpot> BossScene_CamPos;

	PlayerCombatCameraSpot BC_NextSpot;
	bool bBCMove = { false };
	float bBCCameraMove = { 0.f };
	float BCDist = { 1.f };
	int32 BCSceneCameraIndex = { 0 };


	// ----------------------- 5. Ver.BOSS MAP : FIGHT  -----------------------  
	bool		bOriginMovePos = { false };
	float		HiddenAttack_Timer = { 0.f };
	
	float		Rwput_Timer			= { 0.f };
	// 로우풋 할 때 줌 인/아웃
	bool		bRwputZoom			= { false }; 
	// 기존 오리지널 로테이션
	FRotator	OriginRotation;
	// 플레이어 카메라로 변환시켰을 때 세팅하는 용도로 사용
	bool		bPlayerCamSetting	= { false };
	// 조준 시간
	float		AimTimer				= { 0.f };
	// Aim 공격 시작
	bool		bAimAttackStart		= { false };
	// Fov 속도
	float		FovSpeed				= { 50.f };
	// Fove 뒤로 빼기 완료
	bool		bFovBack				= { false };
	// Aim 공격 완료 시
	bool		bAimAttackEnd			= { false };



public :
	UPROPERTY(EditAnywhere)
	TArray<AActor*> CameraScene;

	// 카메라 바꿀 때 사용
	float BossCamera_Timer = { 0.f }; 
	// 현재 카메라 인덱스
	int32 CurrentCameraIndex = 0;
	// 컷신 초기에 카메라 세팅을 0 Camera로 해놨는가
	bool bCameraSetting = { false };
	// 컷신의 보스 애니메이션 Start 여부
	bool CutSceneAnimStart = { false }; 
	// 점점 속도를 높히기 위해서
	float CameraSpeed_Timer = { 0.f };
};
