#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GamePlayTagContainer.h"
#include "R1AnimNotify_SendGameplayEvent.generated.h"

UCLASS()
class LPROJECT_API UR1AnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public :
	UR1AnimNotify_SendGameplayEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected :
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

protected :
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
};
