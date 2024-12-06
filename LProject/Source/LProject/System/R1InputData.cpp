#include "R1InputData.h"

/* 코드를 이용해서 대칭되는 아이를 찾아준다. */
const UInputAction* UR1InputData::FindInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FR1InputAction& Action : InputActions)
	{
		/* nullptr이 아니면서 원하는 태그라면 */
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}
	MSG_BOX(TEXT("태그 없음"))

	return nullptr;
}

