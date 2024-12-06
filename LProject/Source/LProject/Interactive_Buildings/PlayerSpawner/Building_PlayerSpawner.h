// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_PlayerSpawner.generated.h"

UENUM()
enum class ESPAWN_TYPE : int32
{
	BLUE_SPAWN,
	RED_SPAWN,
	GREEN_SPAWN,
	END_SPAWN
};

UCLASS()
class LPROJECT_API ABuilding_PlayerSpawner : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_PlayerSpawner();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit) override;

public: /* For. Getter Setter */
	bool					Get_DialogEnd() { return bDialogEnd; }
	ESPAWN_TYPE		Get_SpawnType() { return Spawn_Type; }
	void					Operate_SpawnParticle();

private :
	void					Operate_BossMap();
	void					Opreate_InGame();

private:
	bool					Find_Widget();
	bool					PlayerPosition_Setting();

	void					Exit_SpawnerWidget();
	void					Stop_SpawnParticle();

public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Dialog_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UDialogEvent_Widget> Dialog_Instance;

protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Type")
	ESPAWN_TYPE		Spawn_Type = { ESPAWN_TYPE::END_SPAWN };

private :
	UPROPERTY()
	class UParticleSystem* SpawnerParticle;
	UPROPERTY()
	TObjectPtr<class UParticleSystemComponent> ParticleSystem;
	UPROPERTY()
	float Particle_Timer = { 0.f };
	UPROPERTY()
	bool bParticleStart = { false };

	UPROPERTY()
	float OpenWidget_Timer = { 0.f };
	UPROPERTY()
	bool bDialogEnd = { false };
	UPROPERTY()
	bool bBossMap_Setting = { false };
};
