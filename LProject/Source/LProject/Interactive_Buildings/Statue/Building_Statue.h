
#pragma once

#include "CoreMinimal.h"
#include "Interactive_Buildings/Interactive_Buildings.h"
#include "Define/R1Define.h"
#include "Building_Statue.generated.h"



UCLASS()
class LPROJECT_API ABuilding_Statue : public AInteractive_Buildings
{
	GENERATED_BODY()

public:
	ABuilding_Statue();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;

public:
	virtual void Tick(float DeltaTime) override;
	
private :
	bool Find_Widget();

public :
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Dialog_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UDialogEvent_Widget> Dialog_Instance;
};
