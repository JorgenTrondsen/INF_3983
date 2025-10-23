#include "GameplayTags.h"

namespace GameplayTags::Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Combat.Data.Damage", "Set By Caller Data Tag For Combat");
}

namespace GameplayTags::Player
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "Player.State.Dead", "Set When Player Is Dead");
}