
#pragma once

#include "CoreMinimal.h"
#include "Monster/R1Monster.h"
#include "R1Monster_Wizard.generated.h"



UCLASS()
class LPROJECT_API AR1Monster_Wizard : public AR1Monster
{
	GENERATED_BODY()

public:
	AR1Monster_Wizard();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual bool Pattern_Function();
	void Impact_PlayerPattern();

private :
	bool Setting_Particle();

};
