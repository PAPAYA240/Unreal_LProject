#pragma once

#include "NativeGameplayTags.h"

namespace R1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_KeyBoardMove);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Turn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Wheel);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Destination);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Energy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Inventory);

	/* Montage*/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_Begin);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_End);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_Attack);
}