// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Building_RestArea.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API ABuilding_RestArea : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_RestArea();

protected:
	virtual void BeginPlay() override;
	bool Find_Widget();

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;


protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> RestArea_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class URestArea_Widget> RestArea_Instance;

protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMesh")
	UStaticMesh* MeshAsset;


};
