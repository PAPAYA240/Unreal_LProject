#pragma once

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	MOVING,
	SKILL,
	DEAD,
	END,
};

UENUM()
enum class EREGION_TYPE
{
	SELISIA_TYPE, // 셀리시아
	HONGYEOP_TYPE, // 홍엽 마을
	DEYRON_TYPE, // 데이론 독숲
	END_TYPE
};

UENUM(BlueprintType)
enum class EMISSION_TYPE : uint8
{
	VISIT_VILLAGEMAN, // 마을 이장 찾아가기
	VISIT_VILLAGEMAN_KILLMONSTER_MISSION, // 마을 이장 : 몬스터 2마리 처치
	VISIT_MARRY, // 메리를 찾으러 가기
	SELISIA_TOWER_BREAK_MISSION, // 셀리시아 마을 정찰탑 부수기
	VISIT_MARRYFATHER, // 메리의 아빠를 만나러 가기
	FIGHT_BOSS,
	END_MISSION
};

UENUM(BlueprintType)
enum class ESCENE : uint8
{
	LOBBY,
	INGAME,
	FIGHT_SCENE,
	BOSS_SCENE,
	END,
};

UENUM(BlueprintType)
enum class EBOSSMAP_CAMERA : uint8
{
	INTOWER_START_BC, // 0 : 상점에 들어가기
	STORE_BC, // 1 : 상점 방문
	FIRST_FIGHT_BC, // 2 : 첫 싸움
	BOSS_SCENE_BC, // 보스 두번 째 컷 씬
	BOSS_FIGHT_BC,//4 : // 보스 싸움
	END_BC
};

UENUM(BlueprintType)
enum class EPLAYER_PATTERN : uint8
{
	DEFAULT,
	MOVING,
	KEYBOARD_MOVING,
	ATTACK,
	HIDDEN_ATTACK,
	AIM_ATTACK,
	RETREAT,
	HIT,
	SHIRK,
	AIM,
	HILL,
	DEAD,

	// Boss
	SHOT_ATTACK,
	END
};

UENUM(BlueprintType)
enum class EMONSTER_PATTERN : uint8
{
	DEFAULT,
	CUT_SCENE, 
	ATTACK,
	HIDDEN_ATTACK,
	AIM_TELEPORT,
	READY_HIT,
	HIT,
	HILL,
	DEAD,
	
	// BOSS
	READY_FIGHT,
	ROWPUT_ATTACK,
	MOVING,
	END
};

UENUM(BlueprintType)
enum class EMONSTER_TYPE : uint8
{
	WIZARD,
	BEE,
	CHEST,
	MUSHROOM,
	TURTLESHELL,
	SWARM, 
	BEHOLDER,// 
	BOSS,
	CRAP,
	LIZARD,
	RAT,
	SPECTER,
	WOLF,
	BLACKKNIGHT,
	END
};

UENUM(BlueprintType)
enum class EFIGHT_AUTHORITY : uint8
{
	PLAYER,
	MONSTER,
	END,
};

UENUM(BlueprintType)
enum class EJOB_TYPE : uint8
{
	LUMBERJACK,
	WIZARD,
	HERBALIST,
	END,
};

UENUM(BlueprintType)
enum class CUSTOMIZE_PLAYER : uint8
{
	FACE_CUSTOM,
	UNDERWARE_CUSTOM,
	HAIR_CUSTOM,
	_CUSTOM,
	TOB_CUSTOM,
	_CUSTOM1,
	_CUSTOM2,
	_CUSTOM3,
	END_CUSTOM,
};

UENUM(BlueprintType)
enum class ABILITY_TYPE : uint8
{
	POWER,
	DEFENCE,
	SPEED,
	INTELLECTUAL,
	HILL,
	LUCKY,
	END,
};

UENUM(BlueprintType)
enum class EITEM_TYPE : uint8
{
	COIN,
	BOOK,
	CANDLE,
	APPLE,
	BOOTS,
	GEM_CRYSTAL,
	GEM_BLUE,
	GEM_GRREN,
	GEM_PURPLE,
	MANA_POTION1,
	MANA_POTION2,
	MANA_POTION3,
	POTION1,
	POTION2,
	POTION3,
	POTION4,
	POTION5,
	MEAT,
	MUSHROOM,
	NECK,
	END
};

UENUM(BlueprintType)
enum class ENPC_TYPE : uint8
{
	POTION_MERCHANT,
	WEAPON_MERCHANT,
	MAGIC_MERCHANT,
	VILLAGEFORMAN_NPC,
	MARRY_NPC,
	MARRY_FATHER,
	DEVIL_NPC, // 악마 NPC
	END_MERCHANT
};

UENUM(BlueprintType)
enum class EBUILDING_TYPE : uint8
{
	PLAYER_START_SPAWNER_BUILIDING,
	HILL_BUILDING,
	TELESCOPE_BUILDING,
	OBJ_SIGN,
	OBJ_CHEST,
	OBJ_NEST,
	OBJ_STOVE,
	OBJ_RUINS,
	EVENT_STATUE_WOODFIRE,
	TENT_BUILDING,
	MUSHROOM_EVENT_BUILDING,
	GRASS_EVENT_BUILDING,
	DEVIL_EVENT,
	MONK_STATUE,
	END_BUILDING
};

struct HoveringType
{
	ABILITY_TYPE abilityType;
	FVector2D HoveringPos;
	bool		bHovering;
};

struct FChaosInfo
{
	class UImage* Chaos_Image;
	int Chaos_Cnt			= 12;
	bool bNextTurn		= false;
	bool bActive			= false;
	FVector2D Last_Position;
};

#define LAST_PLAYER 3
#define MSG_BOX(x) if(GEngine) {GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, x); }