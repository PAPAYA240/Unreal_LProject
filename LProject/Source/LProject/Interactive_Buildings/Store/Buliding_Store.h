// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Buliding_Store.generated.h"


UCLASS()
class LPROJECT_API ABuliding_Store : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuliding_Store();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

public:
	virtual void Tick(float DeltaTime) override;

private :
	bool Find_Widget();
	void Opreate_Widget();
	void CheckCondition();

protected :
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> BuildingEvent_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UBuildingEvent_Widget> BuildingEvent_Instance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> Dialog_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UDialogEvent_Widget> Dialog_Instance;

private :
	UPROPERTY()
	class UParticleSystem* MagicParticle;

	bool bOperateParticle = { false };

};
