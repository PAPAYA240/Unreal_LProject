// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "R1ReconTower.generated.h"



UCLASS()
class LPROJECT_API AR1ReconTower : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	AR1ReconTower();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;


public : /* For. Getter Setter */
	void					Set_FindReconTower(bool _find) { bFindReconTower = _find;  }
	EREGION_TYPE		Get_ReconTowerType() { return Recon_Type;  }
	void					PercentChange();

private : /* For.Function */
	// 1. Setting
	bool			Setting_Scaled();
	bool			Settinng_Widget();
	bool			Setting_BossTower();

	// 2. 작동 Function
	void			Operate_Default();
	void			Operate_Workable(); // Boss Tower를 내리기 위한 함수

	// 3. 세부 Function
	void			Find_ReconTower(); // 탑을 찾았을 때
	void			ReconTower_Rotation(); // 실시간 회전
	void			Opreate_OriginScale(); // 발견 시 크기 키우기
	void			Focus_ReconTower();

	// 4. Widget Function
	void			LookCamera_Widget();


protected : // For. Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recon Type")
	EREGION_TYPE									Recon_Type = { EREGION_TYPE::END_TYPE };

	UPROPERTY(EditAnywhere, Category = "Widget")
	class UWidgetComponent*					HpWidget;

	TObjectPtr<class UR1UserWidget>		ReconHp_Instance = { nullptr };
	
	TObjectPtr<class UMission_Widget> MissionWidget;

protected : // For. Mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent*					 MyMesh;

private :
	TObjectPtr<class ABuilding_Telescope> BossTower = { nullptr };

	float				RotationSpeed			= { 30.f };

	float				EX_Timer					= { 0.f };

	FVector			OriginScale;
	bool				bFindReconTower		= { false };
	bool				bTargetReconTower	= { false }; // 포커스를 잠시 정찰탑에 맞춤
	
	float				WorkableTimer			= { 0 };
	FVector			BT_InitPos				= {};
	
	bool				bWorkable				= { false }; // 몬스터가 꽉참, 작동 가능함
	bool				bOperate				= { false }; // 가동해서 Boss Tower가 내려감

	bool				bJudgCloud				= { false };
};
