//
// published python interface for CyGlobalContext
// Author - Mustafa Thamer
//

#include "CvGameCoreDLL.h"


void CyGlobalContextPythonInterface2(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface2\n");

	x
		// global defines.xml
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   08/02/07                            MRGENIE          */
/*                                                                                              */
/* Return true/false from                                                                       */
/************************************************************************************************/
		.def("getDefineBOOL", &CyGlobalContext::getDefineBOOL, "bool ( string szName)")
/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("isDCM_BATTLE_EFFECTS", &CyGlobalContext::isDCM_BATTLE_EFFECTS, "bool ()")
		.def("getBATTLE_EFFECT_LESS_FOOD", &CyGlobalContext::getBATTLE_EFFECT_LESS_FOOD, "int ()")
		.def("getBATTLE_EFFECT_LESS_PRODUCTION", &CyGlobalContext::getBATTLE_EFFECT_LESS_PRODUCTION, "int ()")
		.def("getBATTLE_EFFECT_LESS_COMMERCE", &CyGlobalContext::getBATTLE_EFFECT_LESS_COMMERCE, "int ()")
		.def("getBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS", &CyGlobalContext::getBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS, "int ()")
		.def("getMAX_BATTLE_TURNS", &CyGlobalContext::getMAX_BATTLE_TURNS, "int ()")

		.def("isDCM_AIR_BOMBING", &CyGlobalContext::isDCM_AIR_BOMBING, "bool ()")
		.def("isDCM_RANGE_BOMBARD", &CyGlobalContext::isDCM_RANGE_BOMBARD, "bool ()")
		.def("getDCM_RB_CITY_INACCURACY", &CyGlobalContext::getDCM_RB_CITY_INACCURACY, "int ()")
		.def("getDCM_RB_CITYBOMBARD_CHANCE", &CyGlobalContext::getDCM_RB_CITYBOMBARD_CHANCE, "int ()")
		.def("isDCM_ATTACK_SUPPORT", &CyGlobalContext::isDCM_ATTACK_SUPPORT, "bool ()")
		.def("isDCM_OPP_FIRE", &CyGlobalContext::isDCM_OPP_FIRE, "bool ()")
		.def("isDCM_ACTIVE_DEFENSE", &CyGlobalContext::isDCM_ACTIVE_DEFENSE, "bool ()")
		.def("isDCM_FIGHTER_ENGAGE", &CyGlobalContext::isDCM_FIGHTER_ENGAGE, "bool ()")

		.def("isDYNAMIC_CIV_NAMES", &CyGlobalContext::isDYNAMIC_CIV_NAMES, "bool ()")

		.def("isIDW_ENABLED", &CyGlobalContext::isIDW_ENABLED, "bool ()")
		.def("getIDW_BASE_COMBAT_INFLUENCE", &CyGlobalContext::getIDW_BASE_COMBAT_INFLUENCE, "float ()")
		.def("getIDW_NO_CITY_DEFENDER_MULTIPLIER", &CyGlobalContext::getIDW_NO_CITY_DEFENDER_MULTIPLIER, "float ()")
		.def("getIDW_FORT_CAPTURE_MULTIPLIER", &CyGlobalContext::getIDW_FORT_CAPTURE_MULTIPLIER, "float ()")
		.def("getIDW_EXPERIENCE_FACTOR", &CyGlobalContext::getIDW_EXPERIENCE_FACTOR, "float ()")
		.def("getIDW_WARLORD_MULTIPLIER", &CyGlobalContext::getIDW_WARLORD_MULTIPLIER, "float ()")
		.def("getIDW_INFLUENCE_RADIUS", &CyGlobalContext::getIDW_INFLUENCE_RADIUS, "int ()")
		.def("getIDW_PLOT_DISTANCE_FACTOR", &CyGlobalContext::getIDW_PLOT_DISTANCE_FACTOR, "float ()")
		.def("getIDW_WINNER_PLOT_MULTIPLIER", &CyGlobalContext::getIDW_WINNER_PLOT_MULTIPLIER, "float ()")
		.def("getIDW_LOSER_PLOT_MULTIPLIER", &CyGlobalContext::getIDW_LOSER_PLOT_MULTIPLIER, "float ()")
		.def("isIDW_EMERGENCY_DRAFT_ENABLED", &CyGlobalContext::isIDW_EMERGENCY_DRAFT_ENABLED, "bool ()")
		.def("getIDW_EMERGENCY_DRAFT_MIN_POPULATION", &CyGlobalContext::getIDW_EMERGENCY_DRAFT_MIN_POPULATION, "int ()")
		.def("getIDW_EMERGENCY_DRAFT_STRENGTH", &CyGlobalContext::getIDW_EMERGENCY_DRAFT_STRENGTH, "float ()")
		.def("getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER", &CyGlobalContext::getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER, "float ()")
		.def("isIDW_NO_BARBARIAN_INFLUENCE", &CyGlobalContext::isIDW_NO_BARBARIAN_INFLUENCE, "bool ()")
		.def("isIDW_NO_NAVAL_INFLUENCE", &CyGlobalContext::isIDW_NO_NAVAL_INFLUENCE, "bool ()")
		.def("isIDW_PILLAGE_INFLUENCE_ENABLED", &CyGlobalContext::isIDW_PILLAGE_INFLUENCE_ENABLED, "bool ()")
		.def("getIDW_BASE_PILLAGE_INFLUENCE", &CyGlobalContext::getIDW_BASE_PILLAGE_INFLUENCE, "float ()")
		.def("getIDW_CITY_TILE_MULTIPLIER", &CyGlobalContext::getIDW_CITY_TILE_MULTIPLIER, "float ()")

		.def("isSS_ENABLED", &CyGlobalContext::isSS_ENABLED, "bool ()")
		.def("isSS_BRIBE", &CyGlobalContext::isSS_BRIBE, "bool ()")
		.def("isSS_ASSASSINATE", &CyGlobalContext::isSS_ASSASSINATE, "bool ()")
/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/

		.def("getDefineINT", &CyGlobalContext::getDefineINT, "int ( string szName )" )
		.def("getDefineFLOAT", &CyGlobalContext::getDefineFLOAT, "float ( string szName )" )
		.def("getDefineSTRING", &CyGlobalContext::getDefineSTRING, "string getDefineSTRING( string szName )" )
		.def("setDefineINT", &CyGlobalContext::setDefineINT, "void ( string szName, int iValue )" )
		.def("setDefineFLOAT", &CyGlobalContext::setDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )
		.def("setDefineSTRING", &CyGlobalContext::setDefineSTRING, "void ( string szName, string szValue )" )
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("setNoUpdateDefineINT", &CyGlobalContext::setNoUpdateDefineINT, "void ( string szName, int iValue )" )
		.def("setNoUpdateDefineFLOAT", &CyGlobalContext::setNoUpdateDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )
		.def("setNoUpdateDefineSTRING", &CyGlobalContext::setNoUpdateDefineSTRING, "void ( string szName, string szValue )" )
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

		.def("getMOVE_DENOMINATOR", &CyGlobalContext::getMOVE_DENOMINATOR, "int ()")
		.def("getNUM_UNIT_PREREQ_OR_BONUSES", &CyGlobalContext::getNUM_UNIT_PREREQ_OR_BONUSES, "int ()")
		.def("getNUM_BUILDING_PREREQ_OR_BONUSES", &CyGlobalContext::getNUM_BUILDING_PREREQ_OR_BONUSES, "int ()")
		.def("getFOOD_CONSUMPTION_PER_POPULATION", &CyGlobalContext::getFOOD_CONSUMPTION_PER_POPULATION, "int ()")
		.def("getMAX_HIT_POINTS", &CyGlobalContext::getMAX_HIT_POINTS, "int ()")
		.def("getHILLS_EXTRA_DEFENSE", &CyGlobalContext::getHILLS_EXTRA_DEFENSE, "int ()")
/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("getPEAK_EXTRA_DEFENSE", &CyGlobalContext::getPEAK_EXTRA_DEFENSE, "int ()")
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/
		.def("getRIVER_ATTACK_MODIFIER", &CyGlobalContext::getRIVER_ATTACK_MODIFIER, "int ()")
		.def("getAMPHIB_ATTACK_MODIFIER", &CyGlobalContext::getAMPHIB_ATTACK_MODIFIER, "int ()")
		.def("getHILLS_EXTRA_MOVEMENT", &CyGlobalContext::getHILLS_EXTRA_MOVEMENT, "int ()")
/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("getPEAK_EXTRA_MOVEMENT", &CyGlobalContext::getPEAK_EXTRA_MOVEMENT, "int ()")
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/
		.def("getMAX_PLOT_LIST_ROWS", &CyGlobalContext::getMAX_PLOT_LIST_ROWS, "int ()")
		.def("getUNIT_MULTISELECT_MAX", &CyGlobalContext::getUNIT_MULTISELECT_MAX, "int ()")
		.def("getPERCENT_ANGER_DIVISOR", &CyGlobalContext::getPERCENT_ANGER_DIVISOR, "int ()")
		.def("getEVENT_MESSAGE_TIME", &CyGlobalContext::getEVENT_MESSAGE_TIME, "int ()")
		.def("getROUTE_FEATURE_GROWTH_MODIFIER", &CyGlobalContext::getROUTE_FEATURE_GROWTH_MODIFIER, "int ()")
		.def("getFEATURE_GROWTH_MODIFIER", &CyGlobalContext::getFEATURE_GROWTH_MODIFIER, "int ()")
		.def("getMIN_CITY_RANGE", &CyGlobalContext::getMIN_CITY_RANGE, "int ()")
		.def("getCITY_MAX_NUM_BUILDINGS", &CyGlobalContext::getCITY_MAX_NUM_BUILDINGS, "int ()")
		.def("getNUM_UNIT_AND_TECH_PREREQS", &CyGlobalContext::getNUM_UNIT_AND_TECH_PREREQS, "int ()")
		.def("getNUM_AND_TECH_PREREQS", &CyGlobalContext::getNUM_AND_TECH_PREREQS, "int ()")
		.def("getNUM_OR_TECH_PREREQS", &CyGlobalContext::getNUM_OR_TECH_PREREQS, "int ()")
		.def("getLAKE_MAX_AREA_SIZE", &CyGlobalContext::getLAKE_MAX_AREA_SIZE, "int ()")
		.def("getNUM_ROUTE_PREREQ_OR_BONUSES", &CyGlobalContext::getNUM_ROUTE_PREREQ_OR_BONUSES, "int ()")
		.def("getNUM_BUILDING_AND_TECH_PREREQS", &CyGlobalContext::getNUM_BUILDING_AND_TECH_PREREQS, "int ()")
		.def("getMIN_WATER_SIZE_FOR_OCEAN", &CyGlobalContext::getMIN_WATER_SIZE_FOR_OCEAN, "int ()")
		.def("getFORTIFY_MODIFIER_PER_TURN", &CyGlobalContext::getFORTIFY_MODIFIER_PER_TURN, "int ()")
		.def("getMAX_CITY_DEFENSE_DAMAGE", &CyGlobalContext::getMAX_CITY_DEFENSE_DAMAGE, "int ()")
		.def("getNUM_CORPORATION_PREREQ_BONUSES", &CyGlobalContext::getNUM_CORPORATION_PREREQ_BONUSES, "int ()")
		.def("getPEAK_SEE_THROUGH_CHANGE", &CyGlobalContext::getPEAK_SEE_THROUGH_CHANGE, "int ()")
		.def("getHILLS_SEE_THROUGH_CHANGE", &CyGlobalContext::getHILLS_SEE_THROUGH_CHANGE, "int ()")
		.def("getSEAWATER_SEE_FROM_CHANGE", &CyGlobalContext::getSEAWATER_SEE_FROM_CHANGE, "int ()")
		.def("getPEAK_SEE_FROM_CHANGE", &CyGlobalContext::getPEAK_SEE_FROM_CHANGE, "int ()")
		.def("getHILLS_SEE_FROM_CHANGE", &CyGlobalContext::getHILLS_SEE_FROM_CHANGE, "int ()")
		.def("getUSE_SPIES_NO_ENTER_BORDERS", &CyGlobalContext::getUSE_SPIES_NO_ENTER_BORDERS, "int ()")

		.def("getCAMERA_MIN_YAW", &CyGlobalContext::getCAMERA_MIN_YAW, "float ()")
		.def("getCAMERA_MAX_YAW", &CyGlobalContext::getCAMERA_MAX_YAW, "float ()")
		.def("getCAMERA_FAR_CLIP_Z_HEIGHT", &CyGlobalContext::getCAMERA_FAR_CLIP_Z_HEIGHT, "float ()")
		.def("getCAMERA_MAX_TRAVEL_DISTANCE", &CyGlobalContext::getCAMERA_MAX_TRAVEL_DISTANCE, "float ()")
		.def("getCAMERA_START_DISTANCE", &CyGlobalContext::getCAMERA_START_DISTANCE, "float ()")
		.def("getAIR_BOMB_HEIGHT", &CyGlobalContext::getAIR_BOMB_HEIGHT, "float ()")
		.def("getPLOT_SIZE", &CyGlobalContext::getPLOT_SIZE, "float ()")
		.def("getCAMERA_SPECIAL_PITCH", &CyGlobalContext::getCAMERA_SPECIAL_PITCH, "float ()")
		.def("getCAMERA_MAX_TURN_OFFSET", &CyGlobalContext::getCAMERA_MAX_TURN_OFFSET, "float ()")
		.def("getCAMERA_MIN_DISTANCE", &CyGlobalContext::getCAMERA_MIN_DISTANCE, "float ()")
		.def("getCAMERA_UPPER_PITCH", &CyGlobalContext::getCAMERA_UPPER_PITCH, "float ()")
		.def("getCAMERA_LOWER_PITCH", &CyGlobalContext::getCAMERA_LOWER_PITCH, "float ()")
		.def("getFIELD_OF_VIEW", &CyGlobalContext::getFIELD_OF_VIEW, "float ()")
		.def("getSHADOW_SCALE", &CyGlobalContext::getSHADOW_SCALE, "float ()")
		.def("getUNIT_MULTISELECT_DISTANCE", &CyGlobalContext::getUNIT_MULTISELECT_DISTANCE, "float ()")

		.def("getMAX_PC_PLAYERS", &CyGlobalContext::getMAX_PC_PLAYERS, "int ()")
		.def("getMAX_PLAYERS", &CyGlobalContext::getMAX_PLAYERS, "int ()")
		.def("getMAX_PC_TEAMS", &CyGlobalContext::getMAX_PC_TEAMS, "int ()")
		.def("getMAX_TEAMS", &CyGlobalContext::getMAX_TEAMS, "int ()")
		.def("getBARBARIAN_PLAYER", &CyGlobalContext::getBARBARIAN_PLAYER, "int ()")
		.def("getBARBARIAN_TEAM", &CyGlobalContext::getBARBARIAN_TEAM, "int ()")
		.def("getNEANDERTHAL_PLAYER", &CyGlobalContext::getNEANDERTHAL_PLAYER, "int ()")
		.def("getNEANDERTHAL_TEAM", &CyGlobalContext::getNEANDERTHAL_TEAM, "int ()")
		.def("getBEAST_PLAYER", &CyGlobalContext::getBEAST_PLAYER, "int ()")
		.def("getBEAST_TEAM", &CyGlobalContext::getBEAST_TEAM, "int ()")
		.def("getPREDATOR_PLAYER", &CyGlobalContext::getPREDATOR_PLAYER, "int ()")
		.def("getPREDATOR_TEAM", &CyGlobalContext::getPREDATOR_TEAM, "int ()")
		.def("getPREY_PLAYER", &CyGlobalContext::getPREY_PLAYER, "int ()")
		.def("getPREY_TEAM", &CyGlobalContext::getPREY_TEAM, "int ()")
		.def("getINSECT_PLAYER", &CyGlobalContext::getINSECT_PLAYER, "int ()")
		.def("getINSECT_TEAM", &CyGlobalContext::getINSECT_TEAM, "int ()")
		.def("getNPC4_PLAYER", &CyGlobalContext::getNPC4_PLAYER, "int ()")
		.def("getNPC4_TEAM", &CyGlobalContext::getNPC4_TEAM, "int ()")
		.def("getNPC3_PLAYER", &CyGlobalContext::getNPC3_PLAYER, "int ()")
		.def("getNPC3_TEAM", &CyGlobalContext::getNPC3_TEAM, "int ()")
		.def("getNPC2_PLAYER", &CyGlobalContext::getNPC2_PLAYER, "int ()")
		.def("getNPC2_TEAM", &CyGlobalContext::getNPC2_TEAM, "int ()")
		.def("getNPC1_PLAYER", &CyGlobalContext::getNPC1_PLAYER, "int ()")
		.def("getNPC1_TEAM", &CyGlobalContext::getNPC1_TEAM, "int ()")
		.def("getNPC0_PLAYER", &CyGlobalContext::getNPC0_PLAYER, "int ()")
		.def("getNPC0_TEAM", &CyGlobalContext::getNPC0_TEAM, "int ()")
		.def("getINVALID_PLOT_COORD", &CyGlobalContext::getINVALID_PLOT_COORD, "int ()")
		.def("getNUM_CITY_PLOTS", &CyGlobalContext::getNUM_CITY_PLOTS, "int ()")
		.def("getCITY_HOME_PLOT", &CyGlobalContext::getCITY_HOME_PLOT, "int ()")
		;
}
