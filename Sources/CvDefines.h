#pragma once

#ifndef CVDEFINES_H
#define CVDEFINES_H

// defines.h

// The following #defines should not be moddable...

#define CVARMY_BREAKSAVE

#define MOVE_IGNORE_DANGER								(0x00000001)
#define MOVE_SAFE_TERRITORY								(0x00000002)
#define MOVE_NO_ENEMY_TERRITORY							(0x00000004)
#define MOVE_DECLARE_WAR								(0x00000008)
#define MOVE_DIRECT_ATTACK								(0x00000010)
#define MOVE_THROUGH_ENEMY								(0x00000020)
#define MOVE_MAX_MOVES									(0x00000040)
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/01/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
// These two flags signal to weight the cost of moving through or adjacent to enemy territory higher
// Used to reduce exposure to attack for approaching enemy cities
#define MOVE_AVOID_ENEMY_WEIGHT_2						(0x00000080)
#define MOVE_AVOID_ENEMY_WEIGHT_3						(0x00000100)
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
//	Koshling - This flag is used to check the turn's immediate movement for ending up next to
//	an enemy unit that we don't have a high chance of surviving an attack from
#define	MOVE_AVOID_ENEMY_UNITS							(0x00000200)
//	Don't go into plots which appear dangerous, even in our own territory unless grouped with a defender
#define	MOVE_WITH_CAUTION								(0x00000400)
#define MOVE_OUR_TERRITORY								(0x00000800)
//	This flag is used internally to signify that the terminal plot causes a war declaration
#define	MOVE_TERMINUS_DECLARES_WAR						(0x00001000)
//	0x2000 currently unused

//	This flag will cause a unit damaged during movement (e.g. - by terrain damage) to
//	pause at the first plot it paths through with no obvious danager, if its damage is >25%
#define MOVE_HEAL_AS_NEEDED25							(0x00004000)
//	Cause units pathing out of owned territory to reconsider so as to guive them a chance to
//	request an escort - only applies to groups that cannot defend
#define MOVE_RECONSIDER_ON_LEAVING_OWNED				(0x00008000)
//	Do not consider paths that allow land unis to cross water (either by loading or water cities)
#define MOVE_NO_LAND_UNITS_ACROSS_WATER					(0x00010000)
//	Pseudo-flag used when constructing recahable plot sets for sea units indicating that
//	adjacent coastal land should be considerd part of the set
#define	MOVE_ALLOW_ADJACENT_COASTAL						(0x00020000)
//	Top byte used to represent max path len
//#define	MOVE_MAX_PATHLEN_MASK							(0xFF000000)
//#define MOVE_MAX_PATHLEN_FROM_INFO(x)					((x) >> 24)
//#define MOVE_FLAGS_FROM_MAX_PATHLEN(x)					((x) << 24)


#define RANDPLOT_LAND									(0x00000001)
#define RANDPLOT_UNOWNED								(0x00000002)
#define RANDPLOT_ADJACENT_UNOWNED						(0x00000004)
#define RANDPLOT_ADJACENT_LAND							(0x00000008)
#define RANDPLOT_PASSIBLE								(0x00000010)
#define RANDPLOT_NOT_VISIBLE_TO_CIV						(0x00000020)
#define RANDPLOT_NOT_CITY								(0x00000040)

//TBANIMAL - the project really starts here
#define MAX_PC_PLAYERS									(40)
#define MAX_PC_TEAMS									(40)
#define BEAST_PLAYER									((PlayerTypes)40)
#define BEAST_TEAM										((TeamTypes)40)
#define PREDATOR_PLAYER									((PlayerTypes)41)
#define PREDATOR_TEAM									((TeamTypes)41)
#define PREY_PLAYER										((PlayerTypes)42)
#define PREY_TEAM										((TeamTypes)42)
#define INSECT_PLAYER									((PlayerTypes)43)
#define INSECT_TEAM										((TeamTypes)43)
#define NPC4_PLAYER										((PlayerTypes)44)
#define NPC4_TEAM										((TeamTypes)44)
#define NPC3_PLAYER										((PlayerTypes)45)
#define NPC3_TEAM										((TeamTypes)45)
#define NPC2_PLAYER										((PlayerTypes)46)
#define NPC2_TEAM										((TeamTypes)46)
#define NPC1_PLAYER										((PlayerTypes)47)
#define NPC1_TEAM										((TeamTypes)47)
#define NPC0_PLAYER										((PlayerTypes)48)
#define NPC0_TEAM										((TeamTypes)48)
#define NEANDERTHAL_PLAYER								((PlayerTypes)49)
#define NEANDERTHAL_TEAM								((TeamTypes)49)
// Toffer - Barbarian player must be last, expected by the exe.
#define BARBARIAN_PLAYER								((PlayerTypes)50)
#define BARBARIAN_TEAM									((TeamTypes)50)
#define MAX_PLAYERS										(51)
#define MAX_TEAMS										(51)

// Char Count limit for edit boxes
//#define PREFERRED_EDIT_CHAR_COUNT						(15)
#define MAX_GAMENAME_CHAR_COUNT							(32)
#define MAX_PLAYERINFO_CHAR_COUNT						(32)
#define MAX_PLAYEREMAIL_CHAR_COUNT						(64)
#define MAX_PASSWORD_CHAR_COUNT							(32)
//#define MAX_GSLOGIN_CHAR_COUNT							(17)
//#define MAX_GSEMAIL_CHAR_COUNT							(50)
//#define MAX_GSPASSWORD_CHAR_COUNT						(30)
//#define MAX_CHAT_CHAR_COUNT								(256)
//#define MAX_ADDRESS_CHAR_COUNT							(64)

#define INVALID_PLOT_COORD								(-(MAX_INT))	// don't use -1 since that is a valid wrap coordinate
#define DIRECTION_RADIUS								(1)
#define DIRECTION_DIAMETER								((DIRECTION_RADIUS * 2) + 1)

#define NUM_CITY_PLOTS_1								(9)
#define NUM_CITY_PLOTS_2								(21)
#define NUM_CITY_PLOTS									(37)
#define CITY_HOME_PLOT									(0)
#define SKIP_CITY_HOME_PLOT								(1)
#define CITY_PLOTS_RADIUS								(3)
#define CITY_PLOTS_DIAMETER								((CITY_PLOTS_RADIUS*2) + 1)

#define GAME_NAME										("Game")

#define LANDSCAPE_FOW_RESOLUTION						(4)

//#define Z_ORDER_LAYER									(-0.1f)
//#define Z_ORDER_LEVEL									(-0.3f)

//#define CIV4_GUID										"civ4bts"
//#define CIV4_PRODUCT_ID									11081
//#define CIV4_NAMESPACE_ID								17
//#define CIV4_NAMESPACE_EXT								"-tk"

#define MAP_TRANSFER_EXT								"_t"

//#define USER_CHANNEL_PREFIX								"#civ4buser!"

#define SETCOLR											L"<color=%d,%d,%d,%d>"
#define ENDCOLR											L"</color>"
#define NEWLINE											L"\n"
#define SEPARATOR										L"\n-----------------------"
// BUG - start
#define DOUBLE_SEPARATOR								L"\n======================="
// BUG - end
#define TEXT_COLOR(szColor)								((int)(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString(szColor)).getColor().r * 255)), ((int)(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString(szColor)).getColor().g * 255)), ((int)(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString(szColor)).getColor().b * 255)), ((int)(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString(szColor)).getColor().a * 255))

//#define CIV4_SHADERS									".\\Shaders\\FXO"
//#define CIV4_ASSETS_PYTHON								".\\Assets\\Python"
//#define CIV4_ASSETS_XML									".\\Assets\\XML"

//#define MAX_PLAYER_NAME_LEN								(64)
//#define MAX_VOTE_CHOICES								(8)
//#define VOTE_TIMEOUT									(600000)	// 10 minute vote timeout - temporary

//#define ANIMATION_DEFAULT								(1)			// Default idle animation

// python module names
#define PYScreensModule									"CvScreensInterface"
#define PYCivModule										"CvAppInterface"
#define PYWorldBuilderModule							"CvWBInterface"
#define PYPopupModule									"CvPopupInterface"
#define PYDiplomacyModule								"CvDiplomacyInterface"
#define PYPitBossModule									"PbMain"
#define PYGameModule									"CvGameInterface"
#define PYEventModule									"CvEventInterface"
#define PYRandomEventModule								"CvRandomEventInterface"

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
// Plot danger cache
#define DANGER_RANGE						(4)
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

// AlbertS2: Save file format version, can be use to make a new dll incompatible with older saves
#define SAVE_FORMAT_VERSION	(4)

#define GC				cvInternalGlobals::getInstance()
#define ARTFILEMGR		CvArtFileMgr::GetInstance()

#define STD_PAIR(T1, T2) \
	std::pair<T1, T2>

#endif	// CVDEFINES_H
