
#pragma once

#include "CoreMinimal.h"
#include "Store/R1Quest_Npc.h"
#include "R1MarryFather.generated.h"

/**
 * 
 */
UCLASS()
class LPROJECT_API AR1MarryFather : public AR1Quest_Npc
{
	GENERATED_BODY()

public:
	AR1MarryFather();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
