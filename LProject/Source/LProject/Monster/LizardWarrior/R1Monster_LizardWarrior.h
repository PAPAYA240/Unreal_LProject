
#pragma once

#include "CoreMinimal.h"
#include "Monster/R1Monster.h"
#include "R1Monster_LizardWarrior.generated.h"


UCLASS()
class LPROJECT_API AR1Monster_LizardWarrior : public AR1Monster
{
	GENERATED_BODY()

public:
	AR1Monster_LizardWarrior();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual bool Pattern_Function();
	virtual void Impact_PlayerPattern();
	
private:
	void		Setting_Information();
	bool		Setting_Weapon();


private :
	UStaticMeshComponent* Left_WeaponMeshComponent;
	UStaticMeshComponent* Right_WeaponMeshComponent;

};
