
#pragma once

#include "CoreMinimal.h"
#include "Trap.h"
#include "Thorns_Trap.generated.h"


UCLASS()
class LPROJECT_API AThorns_Trap : public ATrap
{
	GENERATED_BODY()
	
public:
	AThorns_Trap();

protected:
	virtual void BeginPlay() override;

	bool PlayerInTrap();

public:
	virtual void Tick(float DeltaTime) override;
private :
	bool Operate_Trap(float DeltaTime);


public :
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Dialog_Widget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UDialogEvent_Widget> Dialog_Instance;

	UPROPERTY()
	bool bOperate_Start = { false };

	UPROPERTY()
	bool bOperate_Up = { false };

	UPROPERTY()
	FVector AppearTrap_Position;

	UPROPERTY()
	bool bAppearTrap = { false };
};
