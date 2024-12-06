#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "R1Highlight_Interface.generated.h"

UINTERFACE(MinimalAPI)
class UR1Highlight_Interface : public UInterface
{
	GENERATED_BODY()
};

class LPROJECT_API IR1Highlight_Interface
{
	GENERATED_BODY()

public:
	virtual void Highlight() = 0;
	virtual void UnHighlight() = 0;
};
