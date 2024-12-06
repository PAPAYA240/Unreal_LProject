
#pragma once

#include "CoreMinimal.h"
#include "Store/R1Quest_Npc.h"
#include "R1VillageForeman.generated.h"


UCLASS()
class LPROJECT_API AR1VillageForeman : public AR1Quest_Npc
{
	GENERATED_BODY()

public:
	AR1VillageForeman();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

};
