
#pragma once

#include "CoreMinimal.h"
#include "Player/R1Player.h"
#include "R1Player_Lumberjack.generated.h"

UCLASS()
class LPROJECT_API AR1Player_Lumberjack : public AR1Player
{
	GENERATED_BODY()
	
public :
	AR1Player_Lumberjack();

protected :
	void BeginPlay();
	virtual void Pattern_Function();
	
	bool Setting_PlayerMaterial();

public :
	void Tick(float DeltaTime);


private :
	void Setting_Weapon();
};
