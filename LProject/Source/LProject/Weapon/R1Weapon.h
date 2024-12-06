// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Define/R1Define.h"
#include "R1Weapon.generated.h"

UCLASS()
class LPROJECT_API AR1Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AR1Weapon();

	// Fuction
protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin();

	void EndPlay(const EEndPlayReason::Type EndPlayReason);
	void Set_ShowMesh(bool _visible) { return WeaponMesh->SetVisibility(_visible); }
	void PlayExplosionEffect(FVector Location);


private :
	bool Find_Player();


	// Variable
public :
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class UStaticMeshComponent* WeaponMesh;
	
private :
	FTimerHandle TimerHandle;
	bool bIsOverlapping;
	
protected  :
	UPROPERTY()
	class UParticleSystem* Default_Particle;

	UPROPERTY()
	class UParticleSystem* Explosion_TargetParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SearchRadius = 40.f;

protected:
	TObjectPtr<class AR1Player>			R1Player = { nullptr };
	TObjectPtr<class AMyPlayerState>	CurrentPlayerState = { nullptr };


};
