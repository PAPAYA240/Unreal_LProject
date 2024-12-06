
#pragma once

#include "CoreMinimal.h"
#include "Monster/R1Monster.h"
#include "R1Monster_TurtleShell.generated.h"


UCLASS()
class LPROJECT_API AR1Monster_TurtleShell : public AR1Monster
{
	GENERATED_BODY()
	
public:
	AR1Monster_TurtleShell();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual bool Pattern_Function();
	virtual void Impact_PlayerPattern();

private :
	void Setting_Information();
};
