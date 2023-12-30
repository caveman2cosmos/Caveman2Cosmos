#include "CvGameCoreDLL.h"
#include "CvPythonGlobalContextLoader.h"
#include "CvGameCoreDLL.h"
#include "CyGlobalContext.h"
#include "CvBonusInfo.h"
#include "CvBuildingInfo.h"
#include "CvImprovementInfo.h"
#include "CvHeritageInfo.h"
#include "CvInfos.h"
#include "CvRandom.h"
#include "CyGame.h"
#include "CyGlobalContext.h"
#include "CyMap.h"
#include "CyPlayer.h"
#include "CyTeam.h"
#include "CvDiplomacyClasses.h"
#include "CvPlayerOptionInfo.h"
#include "CvTraitInfo.h"
#include <boost/python/overloads.hpp>


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CyGlobalContext_getInfoTypeForString_overloads, CyGlobalContext::getInfoTypeForString, 1, 2)

void CvPythonGlobalContextLoader::CyGlobalContextPythonInterface1(boost::python::class_<CyGlobalContext>& inst)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface1\n");
	inst
		.def("isDebugBuild", &CyGlobalContext::isDebugBuild, "() - returns true if running a debug build")
		.def("getGame", &CyGlobalContext::getCyGame, boost::python::return_value_policy<boost::python::reference_existing_object>(), "() - CyGame()")
		.def("getMap", &CyGlobalContext::getCyMap, boost::python::return_value_policy<boost::python::reference_existing_object>(), "() - CyMap()")
		.def("getPlayer", &CyGlobalContext::getCyPlayer, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(iPlayer) - iPlayer instance")
		.def("getActivePlayer", &CyGlobalContext::getCyActivePlayer, boost::python::return_value_policy<boost::python::reference_existing_object>(), "() - active player instance")
		.def("getASyncRand", &CyGlobalContext::getCyASyncRand, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Non-Synch'd random #")
		.def("getTeam", &CyGlobalContext::getCyTeam, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(iTeam) - iTeam instance")

		// infos
		.def("getNumEffectInfos", &CyGlobalContext::getNumEffectInfos, "int () - Number of effect infos")
		.def("getEffectInfo", &CyGlobalContext::getEffectInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(int (EffectTypes) eEffectID) - CvInfo for EffectID")

		.def("getNumTerrainInfos", &CyGlobalContext::getNumTerrainInfos, "() - Total Terrain Infos XML\\Terrain\\CIV4TerrainInfos.xml")
		.def("getTerrainInfo", &CyGlobalContext::getTerrainInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(int (TerrainTypes) eTerrainID) - CvInfo for TerrainID")

		.def("getBonusClassInfo", &CyGlobalContext::getBonusClassInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(int (BonusClassTypes) eBonusClassID) - CvInfo for BonusID")

		.def("getNumBonusInfos", &CyGlobalContext::getNumBonusInfos, "() - Total Bonus Infos XML\\Terrain\\CIV4BonusInfos.xml")
		.def("getBonusInfo", &CyGlobalContext::getBonusInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(BonusID) - CvInfo for BonusID")

		.def("getNumMapBonuses", &CyGlobalContext::getNumMapBonuses, "() - Total map Bonuses")
		.def("getMapBonus", &CyGlobalContext::getMapBonus, "(mapBonusIndex) - BonusType for mapBonusIndex")

		.def("getNumFeatureInfos", &CyGlobalContext::getNumFeatureInfos, "() - Total Feature Infos XML\\Terrain\\CIV4FeatureInfos.xml")
		.def("getFeatureInfo", &CyGlobalContext::getFeatureInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(FeatureID) - CvInfo for FeatureID")

		.def("getNumUpkeepInfos", &CyGlobalContext::getNumUpkeepInfos, "int () - Number of upkeep infos")
		.def("getUpkeepInfo", &CyGlobalContext::getUpkeepInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(UpkeepInfoID) - CvInfo for upkeep info")

		.def("getNumCultureLevelInfos", &CyGlobalContext::getNumCultureLevelInfos, "int () - Number of culture level infos")
		.def("getCultureLevelInfo", &CyGlobalContext::getCultureLevelInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CultureLevelID) - CvInfo for CultureLevelID")

		.def("getNumEraInfos", &CyGlobalContext::getNumEraInfos, "int () - Number of era infos")
		.def("getEraInfo", &CyGlobalContext::getEraInfo, boost::python::return_value_policy<boost::python::reference_existing_object>())

		.def("getNumWorldInfos", &CyGlobalContext::getNumWorldInfos, "int () - Number of world infos")
		.def("getWorldInfo", &CyGlobalContext::getWorldInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "CvWorldInfo - (WorldTypeID)")

		.def("getNumClimateInfos", &CyGlobalContext::getNumClimateInfos, "int () - Number of climate infos")
		.def("getClimateInfo", &CyGlobalContext::getClimateInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "CvClimateInfo - (ClimateTypeID)")

		.def("getNumSeaLevelInfos", &CyGlobalContext::getNumSeaLevelInfos, "int () - Number of seal level infos")
		.def("getSeaLevelInfo", &CyGlobalContext::getSeaLevelInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "CvSeaLevelInfo - (SeaLevelTypeID)")

		.def("getNumPlayableCivilizationInfos", &CyGlobalContext::getNumPlayableCivilizationInfos, "() - Total # of Playable Civs")
		.def("getNumCivilizationInfos", &CyGlobalContext::getNumCivilizatonInfos, "() - Total Civilization Infos XML\\Civilizations\\CIV4CivilizationInfos.xml")
		.def("getCivilizationInfo", &CyGlobalContext::getCivilizationInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CivilizationID) - CvInfo for CivilizationID")

		.def("getNumLeaderHeadInfos", &CyGlobalContext::getNumLeaderHeadInfos, "() - Total LeaderHead Infos XML\\Civilizations\\CIV4LeaderHeadInfos.xml")
		.def("getLeaderHeadInfo", &CyGlobalContext::getLeaderHeadInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(LeaderHeadID) - CvInfo for LeaderHeadID")

		.def("getNumTraitInfos", &CyGlobalContext::getNumTraitInfos, "() - Total Civilization Infos XML\\Civilizations\\CIV4TraitInfos.xml")
		.def("getTraitInfo", &CyGlobalContext::getTraitInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(TraitID) - CvInfo for TraitID")

		.def("getNumUnitInfos", &CyGlobalContext::getNumUnitInfos, "() - Total Unit Infos XML\\Units\\CIV4UnitInfos.xml")
		.def("getUnitInfo", &CyGlobalContext::getUnitInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(UnitID) - CvInfo for UnitID")

		.def("getNumSpecialUnitInfos", &CyGlobalContext::getNumSpecialUnitInfos, "() - Total SpecialUnit Infos XML\\Units\\CIV4SpecialUnitInfos.xml")
		.def("getSpecialUnitInfo", &CyGlobalContext::getSpecialUnitInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(UnitID) - CvInfo for UnitID")

		.def("getYieldInfo", &CyGlobalContext::getYieldInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(YieldID) - CvInfo for YieldID")

		.def("getCommerceInfo", &CyGlobalContext::getCommerceInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CommerceID) - CvInfo for CommerceID")

		.def("getNumRouteInfos", &CyGlobalContext::getNumRouteInfos, "() - Total Route Infos XML\\Misc\\CIV4RouteInfos.xml")
		.def("getRouteInfo", &CyGlobalContext::getRouteInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(RouteID) - CvInfo for RouteID")

		.def("getNumImprovementInfos", &CyGlobalContext::getNumImprovementInfos, "() - Total Improvement Infos XML\\Terrain\\CIV4ImprovementInfos.xml")
		.def("getImprovementInfo", &CyGlobalContext::getImprovementInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(ImprovementID) - CvInfo for ImprovementID")

		.def("getNumGoodyInfos", &CyGlobalContext::getNumGoodyInfos, "() - Total Goody Infos XML\\GameInfo\\CIV4GoodyInfos.xml")
		.def("getGoodyInfo", &CyGlobalContext::getGoodyInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(GoodyID) - CvInfo for GoodyID")

		.def("getNumBuildInfos", &CyGlobalContext::getNumBuildInfos, "() - Total Build Infos XML\\Units\\CIV4BuildInfos.xml")
		.def("getBuildInfo", &CyGlobalContext::getBuildInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(BuildID) - CvInfo for BuildID")

		.def("getNumHandicapInfos", &CyGlobalContext::getNumHandicapInfos, "() - Total Handicap Infos XML\\GameInfo\\CIV4HandicapInfos.xml")
		.def("getHandicapInfo", &CyGlobalContext::getHandicapInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(HandicapID) - CvInfo for HandicapID")

		.def("getNumGameSpeedInfos", &CyGlobalContext::getNumGameSpeedInfos, "() - Total Game speed Infos XML\\GameInfo\\CIV4GameSpeedInfo.xml")
		.def("getGameSpeedInfo", &CyGlobalContext::getGameSpeedInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(GameSpeed Info) - CvInfo for GameSpeedID")

		.def("getNumBuildingInfos", &CyGlobalContext::getNumBuildingInfos, "() - Total Building Infos XML\\Buildings\\CIV4BuildingInfos.xml")
		.def("getBuildingInfo", &CyGlobalContext::getBuildingInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(BuildingID) - CvInfo for BuildingID")

		.def("getNumUnitCombatInfos", &CyGlobalContext::getNumUnitCombatInfos, "() - Total Unit Combat Infos XML\\Units\\CIV4UnitCombatInfos.xml")
		.def("getUnitCombatInfo", &CyGlobalContext::getUnitCombatInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(UnitCombatID) - CvInfo for UnitCombatID")

		.def("getDomainInfo", &CyGlobalContext::getDomainInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(DomainID) - CvInfo for DomainID")

		.def("getNumActionInfos", &CyGlobalContext::getNumActionInfos, "() - Total Action Infos XML\\Units\\CIV4ActionInfos.xml")
		.def("getActionInfo", &CyGlobalContext::getActionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(ActionID) - CvInfo for ActionID")

		.def("getNumPromotionLineInfos", &CyGlobalContext::getNumPromotionLineInfos, "() - Total Promotion Line Infos XML\\Units\\CIV4PromotionLineInfos.xml")
		.def("getPromotionLineInfo", &CyGlobalContext::getPromotionLineInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(PromotionLineID) - CvInfo for PromotionLineID")

		//.def("getNumIdeaClassInfos", &CyGlobalContext::getNumIdeaClassInfos, "() - Total Idea Class Infos XML\\GameInfo\\CIV4IdeaClassInfos.xml")
		//.def("getIdeaClassInfo", &CyGlobalContext::getIdeaClassInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(IdeaClassID) - CvInfo for IdeaClassID")

		//.def("getNumIdeaInfos", &CyGlobalContext::getNumIdeaInfos, "() - Total Idea Infos XML\\GameInfo\\CIV4IdeaInfos.xml")
		//.def("getIdeaInfo", &CyGlobalContext::getIdeaInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(IdeaID) - CvInfo for IdeaID")

		//.def("getNumInvisibleInfos", &CyGlobalContext::getNumInvisibleInfos, "() - Total Invisible Infos XML\\GameInfo\\CIV4InvisibleInfos.xml")
		//.def("getInvisibleInfo", &CyGlobalContext::getInvisibleInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(InvisibleID) - CvInfo for InvisibleID")

		//.def("getNumTraitOptionEditsInfos", &CyGlobalContext::getNumTraitOptionEditsInfos, "() - Total Trait Option Edits Infos XML\\Traits\\CIV4TraitOptionEditsInfos.xml")
		//.def("getTraitOptionEditsInfo", &CyGlobalContext::getTraitOptionEditsInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(TraitOptionEditsID) - CvInfo for TraitOptionEditsID")

		.def("setIsBug", &CyGlobalContext::setIsBug, "void () - init BUG on dll side")
		.def("refreshOptionsBUG", &CyGlobalContext::refreshOptionsBUG, "void () - refresh some key BUG options")
		;
}
void CvPythonGlobalContextLoader::CyGlobalContextPythonInterface2(boost::python::class_<CyGlobalContext>& inst)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface2\n");
	inst
		// global defines.xml
		.def("isDCM_AIR_BOMBING", &CyGlobalContext::isDCM_AIR_BOMBING, "bool ()")
		.def("isDCM_RANGE_BOMBARD", &CyGlobalContext::isDCM_RANGE_BOMBARD, "bool ()")
		.def("isDCM_OPP_FIRE", &CyGlobalContext::isDCM_OPP_FIRE, "bool ()")
		.def("isDCM_ACTIVE_DEFENSE", &CyGlobalContext::isDCM_ACTIVE_DEFENSE, "bool ()")
		.def("isDCM_FIGHTER_ENGAGE", &CyGlobalContext::isDCM_FIGHTER_ENGAGE, "bool ()")

		.def("isIDW_ENABLED", &CyGlobalContext::isIDW_ENABLED, "bool ()")
		.def("isIDW_EMERGENCY_DRAFT_ENABLED", &CyGlobalContext::isIDW_EMERGENCY_DRAFT_ENABLED, "bool ()")
		.def("isIDW_NO_BARBARIAN_INFLUENCE", &CyGlobalContext::isIDW_NO_BARBARIAN_INFLUENCE, "bool ()")
		.def("isIDW_NO_NAVAL_INFLUENCE", &CyGlobalContext::isIDW_NO_NAVAL_INFLUENCE, "bool ()")
		.def("isIDW_PILLAGE_INFLUENCE_ENABLED", &CyGlobalContext::isIDW_PILLAGE_INFLUENCE_ENABLED, "bool ()")

		.def("isSS_ENABLED", &CyGlobalContext::isSS_ENABLED, "bool ()")
		.def("isSS_BRIBE", &CyGlobalContext::isSS_BRIBE, "bool ()")
		.def("isSS_ASSASSINATE", &CyGlobalContext::isSS_ASSASSINATE, "bool ()")

		.def("getDefineINT", &CyGlobalContext::getDefineINT, "int ( string szName )")
		.def("getDefineFLOAT", &CyGlobalContext::getDefineFLOAT, "float ( string szName )")
		.def("setDefineINT", &CyGlobalContext::setDefineINT, "void ( string szName, int iValue )")
		.def("setDefineFLOAT", &CyGlobalContext::setDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )")
		.def("setNoUpdateDefineFLOAT", &CyGlobalContext::setNoUpdateDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )")

		.def("getMAX_PC_PLAYERS", &CyGlobalContext::getMAX_PC_PLAYERS, "int ()")
		.def("getMAX_PLAYERS", &CyGlobalContext::getMAX_PLAYERS, "int ()")
		.def("getMAX_PC_TEAMS", &CyGlobalContext::getMAX_PC_TEAMS, "int ()")
		.def("getMAX_TEAMS", &CyGlobalContext::getMAX_TEAMS, "int ()")
		.def("getBARBARIAN_PLAYER", &CyGlobalContext::getBARBARIAN_PLAYER, "int ()")
		.def("getNUM_CITY_PLOTS", &CyGlobalContext::getNUM_CITY_PLOTS, "int ()")

#define EXPOSE_FUNC(unused_, VAR) \
	_EXPOSE_FUNC(get##VAR)

#define _EXPOSE_FUNC(name) \
		.def(#name, &CyGlobalContext::name)

		DO_FOR_EACH_EXPOSED_INT_GLOBAL_DEFINE(EXPOSE_FUNC)
		DO_FOR_EACH_EXPOSED_INFO_TYPE(EXPOSE_FUNC)
		;
}
void CvPythonGlobalContextLoader::CyGlobalContextPythonInterface3(boost::python::class_<CyGlobalContext>& inst)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface3\n");

	inst
		.def("getMapInfo", &CyGlobalContext::getMapInfo, boost::python::return_value_policy<boost::python::reference_existing_object>())
		.def("switchMap", &CyGlobalContext::switchMap, "void (int)")
		.def("getMapByIndex", &CyGlobalContext::getMapByIndex, boost::python::return_value_policy<boost::python::reference_existing_object>(), "CyMap (int)")

		.def("getAttitudeInfo", &CyGlobalContext::getAttitudeInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "AttitudeInfo (int id)")
		.def("getMemoryInfo", &CyGlobalContext::getMemoryInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "MemoryInfo (int id)")

		.def("getPlayerOptionsInfo", &CyGlobalContext::getPlayerOptionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(PlayerOptionsInfoID) - PlayerOptionsInfo for PlayerOptionsInfo")

		.def("getGraphicOptionsInfo", &CyGlobalContext::getGraphicOptionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(GraphicOptionsInfoID) - GraphicOptionsInfo for GraphicOptionsInfo")

		.def("getNumHurryInfos", &CyGlobalContext::getNumHurryInfos, "() - Total Hurry Infos")

		.def("getNumConceptInfos", &CyGlobalContext::getNumConceptInfos, "int () - NumConceptInfos")
		.def("getConceptInfo", &CyGlobalContext::getConceptInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Concept Info () - Returns info object")

		.def("getNumNewConceptInfos", &CyGlobalContext::getNumNewConceptInfos, "int () - NumNewConceptInfos")
		.def("getNewConceptInfo", &CyGlobalContext::getNewConceptInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "New Concept Info () - Returns info object")

		.def("getNumCalendarInfos", &CyGlobalContext::getNumCalendarInfos, "int () - Returns NumCalendarInfos")
		.def("getCalendarInfo", &CyGlobalContext::getCalendarInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "CalendarInfo () - Returns Info object")

		.def("getNumGameOptionInfos", &CyGlobalContext::getNumGameOptionInfos, "int () - Returns NumGameOptionInfos")
		.def("getGameOptionInfo", &CyGlobalContext::getGameOptionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "GameOptionInfo () - Returns Info object")

		.def("getNumMPOptionInfos", &CyGlobalContext::getNumMPOptionInfos, "int () - Returns NumMPOptionInfos")
		.def("getMPOptionInfo", &CyGlobalContext::getMPOptionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "MPOptionInfo () - Returns Info object")

		.def("getNumForceControlInfos", &CyGlobalContext::getNumForceControlInfos, "int () - Returns NumForceControlInfos")
		.def("getForceControlInfo", &CyGlobalContext::getForceControlInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "ForceControlInfo () - Returns Info object")

		.def("getNumSeasonInfos", &CyGlobalContext::getNumSeasonInfos, "int () - Returns NumSeasonInfos")
		.def("getSeasonInfo", &CyGlobalContext::getSeasonInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "SeasonInfo () - Returns Info object")

		.def("getNumDenialInfos", &CyGlobalContext::getNumDenialInfos, "int () - Returns NumDenialInfos")
		.def("getDenialInfo", &CyGlobalContext::getDenialInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "DenialInfo () - Returns Info object")
		;
}
void CvPythonGlobalContextLoader::CyGlobalContextPythonInterface4(boost::python::class_<CyGlobalContext>& inst)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface4\n");
	inst
		.def("getNumMissionInfos", &CyGlobalContext::getNumMissionInfos, "() - Total Mission Infos XML\\Units\\CIV4MissionInfos.xml")
		.def("getMissionInfo", &CyGlobalContext::getMissionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(MissionID) - CvInfo for MissionID")

		.def("getNumCommandInfos", &CyGlobalContext::getNumCommandInfos, "() - Total Command Infos XML\\Units\\CIV4CommandInfos.xml")
		.def("getCommandInfo", &CyGlobalContext::getCommandInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CommandID) - CvInfo for CommandID")

		.def("getNumControlInfos", &CyGlobalContext::getNumControlInfos, "() - Total Control Infos XML\\Units\\CIV4ControlInfos.xml")
		.def("getControlInfo", &CyGlobalContext::getControlInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(ControlID) - CvInfo for ControlID")

		.def("getNumPromotionInfos", &CyGlobalContext::getNumPromotionInfos, "() - Total Promotion Infos XML\\Units\\CIV4PromotionInfos.xml")
		.def("getPromotionInfo", &CyGlobalContext::getPromotionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(PromotionID) - CvInfo for PromotionID")

		.def("getNumTechInfos", &CyGlobalContext::getNumTechInfos, "() - Total Technology Infos XML\\Technologies\\CIV4TechInfos.xml")
		.def("getTechInfo", &CyGlobalContext::getTechInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(TechID) - CvInfo for TechID")

		.def("getNumSpecialBuildingInfos", &CyGlobalContext::getNumSpecialBuildingInfos, "() - Total Special Building Infos")
		.def("getSpecialBuildingInfo", &CyGlobalContext::getSpecialBuildingInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(SpecialBuildingID) - CvInfo for SpecialBuildingID")

		.def("getNumReligionInfos", &CyGlobalContext::getNumReligionInfos, "() - Total Religion Infos XML\\GameInfo\\CIV4ReligionInfos.xml")
		.def("getReligionInfo", &CyGlobalContext::getReligionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(ReligionID) - CvInfo for ReligionID")

		.def("getNumHeritageInfos", &CyGlobalContext::getNumHeritageInfos, "() - Total Heritage Infos XML\\GameInfo\\HeritageInfos.xml")
		.def("getHeritageInfo", &CyGlobalContext::getHeritageInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(HeritageID) - CvInfo for HeritageID")

		.def("getNumCorporationInfos", &CyGlobalContext::getNumCorporationInfos, "() - Total Corporation Infos XML\\GameInfo\\CIV4CorporationInfos.xml")
		.def("getCorporationInfo", &CyGlobalContext::getCorporationInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CorporationID) - CvInfo for CorporationID")

		.def("getNumVictoryInfos", &CyGlobalContext::getNumVictoryInfos, "() - Total Victory Infos XML\\GameInfo\\CIV4VictoryInfos.xml")
		.def("getVictoryInfo", &CyGlobalContext::getVictoryInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(VictoryID) - CvInfo for VictoryID")

		.def("getNumSpecialistInfos", &CyGlobalContext::getNumSpecialistInfos, "() - Total Specialist Infos XML\\Units\\CIV4SpecialistInfos.xml")
		.def("getSpecialistInfo", &CyGlobalContext::getSpecialistInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(SpecialistID) - CvInfo for SpecialistID")

		.def("getNumCivicOptionInfos", &CyGlobalContext::getNumCivicOptionInfos, "() - Total Civic Infos XML\\Misc\\CIV4CivicOptionInfos.xml")
		.def("getCivicOptionInfo", &CyGlobalContext::getCivicOptionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CivicID) - CvInfo for CivicID")

		.def("getNumCivicInfos", &CyGlobalContext::getNumCivicInfos, "() - Total Civic Infos XML\\Misc\\CIV4CivicInfos.xml")
		.def("getCivicInfo", &CyGlobalContext::getCivicInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(CivicID) - CvInfo for CivicID")

		.def("getNumDiplomacyInfos", &CyGlobalContext::getNumDiplomacyInfos, "() - Total diplomacy Infos XML\\GameInfo\\CIV4DiplomacyInfos.xml")
		.def("getDiplomacyInfo", &CyGlobalContext::getDiplomacyInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(DiplomacyID) - CvInfo for DiplomacyID")

		.def("getNumProjectInfos", &CyGlobalContext::getNumProjectInfos, "() - Total Project Infos XML\\GameInfo\\CIV4ProjectInfos.xml")
		.def("getProjectInfo", &CyGlobalContext::getProjectInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(ProjectID) - CvInfo for ProjectID")

		.def("getNumVoteInfos", &CyGlobalContext::getNumVoteInfos, "() - Total VoteInfos")
		.def("getVoteInfo", &CyGlobalContext::getVoteInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(VoteID) - CvInfo for VoteID")

		.def("getNumProcessInfos", &CyGlobalContext::getNumProcessInfos, "() - Total ProcessInfos")
		.def("getProcessInfo", &CyGlobalContext::getProcessInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(ProcessID) - CvInfo for ProcessID")

		.def("getNumEmphasizeInfos", &CyGlobalContext::getNumEmphasizeInfos, "() - Total EmphasizeInfos")
		.def("getEmphasizeInfo", &CyGlobalContext::getEmphasizeInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(EmphasizeID) - CvInfo for EmphasizeID")

		.def("getHurryInfo", &CyGlobalContext::getHurryInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "(HurryID) - CvInfo for HurryID")

		.def("getUnitAIInfo", &CyGlobalContext::getUnitAIInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "UnitAIInfo (int id)")

		.def("getColorInfo", &CyGlobalContext::getColorInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "ColorInfo (int id)")

		.def("getAdvisorInfo", &CyGlobalContext::getAdvisorInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "AdvisorInfo (int id)")

		.def("getInfoTypeForString", &CyGlobalContext::getInfoTypeForString, CyGlobalContext_getInfoTypeForString_overloads())

		.def("getNumFlavorTypes", &CyGlobalContext::getNumFlavorTypes)
		.def("getFlavorType", &CyGlobalContext::getFlavorType)
		.def("getFlavorTypes", &CyGlobalContext::getFlavorTypes)

		.def("getNumPlayerColorInfos", &CyGlobalContext::getNumPlayerColorInfos, "int () - Returns number of PlayerColorInfos")
		.def("getPlayerColorInfo", &CyGlobalContext::getPlayerColorInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "PlayerColorInfo (int id)")

		.def("getNumEventTriggerInfos", &CyGlobalContext::getNumEventTriggerInfos, "int () - Returns number of EventTriggerInfos")
		.def("getEventTriggerInfo", &CyGlobalContext::getEventTriggerInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "EventTriggerInfo () - Returns info object")

		.def("getNumEventInfos", &CyGlobalContext::getNumEventInfos, "int () - Returns number of EventInfos")
		.def("getEventInfo", &CyGlobalContext::getEventInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "EventInfo () - Returns info object")

		.def("getNumEspionageMissionInfos", &CyGlobalContext::getNumEspionageMissionInfos, "int () - Returns number of EspionageMissionInfos")
		.def("getEspionageMissionInfo", &CyGlobalContext::getEspionageMissionInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "EspionageMissionInfo () - Returns info object")

		.def("getNumMainMenus", &CyGlobalContext::getNumMainMenus, "int () - Returns number")
		.def("getMainMenus", &CyGlobalContext::getMainMenus, boost::python::return_value_policy<boost::python::reference_existing_object>(), "MainMenus () - Returns info object")

		.def("getNumVoteSourceInfos", &CyGlobalContext::getNumVoteSourceInfos, "int ()")
		.def("getVoteSourceInfo", &CyGlobalContext::getVoteSourceInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns info object")

		.def("getNumVoteSourceInfos", &CyGlobalContext::getNumVoteSourceInfos, "int ()")
		.def("getVoteSourceInfo", &CyGlobalContext::getVoteSourceInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns info object")

		.def("getArtStyleTypes", &CyGlobalContext::getArtStyleTypes, "string () - Returns enum string")

		.def("getNumPropertyInfos", &CyGlobalContext::getNumPropertyInfos, "int () - Returns number of PropertyInfos")
		.def("getPropertyInfo", &CyGlobalContext::getPropertyInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "PropertyInfo (int)")
	;
}
