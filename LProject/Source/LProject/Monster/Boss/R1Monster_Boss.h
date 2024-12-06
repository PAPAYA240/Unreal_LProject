// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/R1Monster.h"
#include "R1Monster_Boss.generated.h"


UCLASS()
class LPROJECT_API AR1Monster_Boss : public AR1Monster
{
	GENERATED_BODY()

public:
	AR1Monster_Boss();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual bool Pattern_Function();
	virtual void Impact_PlayerPattern();

	bool Setting_MonsterInfo();
	bool Setting_Widget();

public :  //===========  For. Getter Setter  ================
	// Rowt 공격이 끝났는가?
	bool			IsRowAttackEnd()					{ return bRowPutAttack_End; }
	FVector		Get_TargetPosition()				{ return Boss_InitPosition;  }
	// Player Aim 시 정확도
	void			Set_AimPercent(int32 _percent) { AimPercent = _percent; }
	// 보스 스켈레톤 메시를 가져감
	class USkeletalMeshComponent* Get_BossSkeletalMesh() { return BossSkeletalMesh; }

private:  //================  For.Widget 함수 ================
	void		CrosshairWidget(); 
	void		CrosshairScore(); // Aim Attack 점수내기
	void		CrosshairHideAndBlink(); // 깜빡거리고 끄기

private :  //================  For.행동 처리 ================
	bool		PlayerTarget_Setting();
	// 공중 부양
	bool		Ready_Levitation();
	// 빌딩 주변 돌기
	bool		AroundBuilding(); 
	void		Stop_AroundBuilding();



private : //================ For.Attack  ================
	void		Select_Attack();
	bool		Hidden_Attack();
	bool		Default_StartAttack();
	bool		RowPut_Attack();


protected :  //================ For. Particle ================
	UPROPERTY()
	class UNiagaraSystem* HiddenAttack_Particle;
	class UNiagaraComponent* NiagaraComponent;

	TObjectPtr<class UParticleSystemComponent> Rwput_ParticleComponent = { nullptr };
	TObjectPtr<UParticleSystem> Rwput_Particle;
	TObjectPtr<UParticleSystem> Rwput_ReadyParticle;

	TObjectPtr<class UParticleSystemComponent> Attack_ParticleComponent = { nullptr };
	TArray<class UParticleSystemComponent*> Array_AttackParticle;
	

protected :  //================  For. Widget ================
	// Crosshair
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget>		Crosshair_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UR1UserWidget*				Crosshair_Instance;


private :
	class USkeletalMeshComponent* BossSkeletalMesh;

	UPROPERTY()
	float			Speed = { 500.f };
	float			RotationSpeed = { 150.f };
	UPROPERTY()
	bool			bBossStart = { false };
	UPROPERTY()
	bool			bAroundCenter = { false };
	UPROPERTY()
	float			ReadyAround_Timer = { 0.f };

	// 몇 번 공격했는가?
	UPROPERTY()
	int32 AttackCnt					= { 0 };
	// 공격이 끝나면 몇 초 기다려 줄 것인가?
	UPROPERTY()
	float	AttackEnd_Timer		= { 0.f };
	/* ================ Attack 변수 ================*/
private :
	// 이전 권한을 확인한다.
	EFIGHT_AUTHORITY		PrevAuthority = { EFIGHT_AUTHORITY::END  };



private :		 // Hidden Attack 변수
	UPROPERTY()
	FVector		Boss_InitPosition;
	UPROPERTY()
	FVector		Boss_TargetPosition;
	UPROPERTY()
	FVector		Player_LastPos;
	UPROPERTY()
	float			HiddenAttack_Timer = { 0.f };

private:		// + Default도
	UPROPERTY()
	float			TargetPos_Timer = { 0.f };
	UPROPERTY()
	bool			bTargetPos = { false };

private:		// RowPut Attack 변수
	UPROPERTY()
	float			ReadyRowPut_Timer = { 0.f };
	UPROPERTY()
	bool			bReadyRowPut_Position = { false };
	UPROPERTY()
	bool			bRowPutAttack_End = { false };


private:		// Default Star Attack 변수
	UPROPERTY()
	bool			bSpawnAttackParticle = { false };
	UPROPERTY()
	float			DefaultParticle_Timer = { 0.f };
	UPROPERTY()
	int32			ParticleAttack_Idx = { 0 };
	UPROPERTY()
	float			DefaultAttack_Distance = { 2000.f };

private:
	UPROPERTY()
	FVector		Building_Center; // 빌딩 센터
	UPROPERTY()
	float			BetweenBuilding_Distance = { 1500.f };//빌딩과의 거리
	UPROPERTY()
	bool			bDefaultAttack = { false };

private :		 // Widget 관련 함수
	// 에임을 몇 퍼센트나 맞췄는가?
	UPROPERTY()
	int32		AimPercent			= { 0 }; 
	// 에임을 완료하였는가?
	UPROPERTY()
	bool		bAimEnd				= { false };
	// Aim Score 글자 깜빡임
	UPROPERTY()
	float		AimScoreBlinking_Timer	= { 0.f };
	// Aim Score 글자 깜빡임
	UPROPERTY()
	bool		bAimScoreBlinking = { false };
};
