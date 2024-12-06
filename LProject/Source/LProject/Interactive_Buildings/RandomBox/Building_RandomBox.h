// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_RandomBox.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API ABuilding_RandomBox : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_RandomBox();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

private:
	bool			Find_Widget();

	void			Floundering();

protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> RandomBox_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class URandomBox_Widget> RandomBox_Instance;

private:
	UPROPERTY()
	bool				bFloundering = { false };
	UPROPERTY()
	FVector			Origin_Pos = {};

};
