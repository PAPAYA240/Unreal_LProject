#pragma once

#include "CoreMinimal.h"
#include "Monster/R1Monster.h"
#include "R1Monster_Bee.generated.h"


UCLASS()
class LPROJECT_API AR1Monster_Bee : public AR1Monster
{
	GENERATED_BODY()

public:
	AR1Monster_Bee();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual bool Pattern_Function();
	virtual void Impact_PlayerPattern();

private :
	void	Setting_Information();

};
