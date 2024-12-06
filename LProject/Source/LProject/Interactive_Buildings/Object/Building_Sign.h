// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_Sign.generated.h"


UCLASS()
class LPROJECT_API ABuilding_Sign : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_Sign();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

private :
	bool		Setting_WidgetOption();
	void		Return();
	void		Operate_MiniGame();

protected : 
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Dialog_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UHintDialog_Widget> Dialog_Instance;

private :
	TArray<class AEnvironment_Cliff*> Cliff_Array;

	float Failed_ChangeCam_Timer		= { 0.f };

	bool bOperate							= { false };
	bool bFailed							= { false };
	bool bCamOriginPos					= { false };
	FVector Cam_InitPos					= {};

};
