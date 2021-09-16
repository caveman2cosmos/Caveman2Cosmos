#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvRandom.h"
#include "CyGame.h"
#include "CyGlobalContext.h"
#include "CyMap.h"
#include "CyPlayer.h"
#include "CyTeam.h"

//
// published python interface for CyGlobalContext
// Author - Mustafa Thamer
//

void CyGlobalContextPythonInterface1(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface1\n");
	x
		.def("isDebugBuild", &CyGlobalContext::isDebugBuild, "() - returns true if running a debug build")
		.def("getGame", &CyGlobalContext::getCyGame, python::return_value_policy<python::reference_existing_object>(), "() - CyGame()")
		.def("getMap", &CyGlobalContext::getCyMap, python::return_value_policy<python::reference_existing_object>(), "() - CyMap()")
		.def("getPlayer", &CyGlobalContext::getCyPlayer, python::return_value_policy<python::reference_existing_object>(), "(iPlayer) - iPlayer instance")
		.def("getActivePlayer", &CyGlobalContext::getCyActivePlayer, python::return_value_policy<python::reference_existing_object>(), "() - active player instance")
		.def("getASyncRand", &CyGlobalContext::getCyASyncRand, python::return_value_policy<python::reference_existing_object>(), "Non-Synch'd random #")
		.def("getTeam", &CyGlobalContext::getCyTeam, python::return_value_policy<python::reference_existing_object>(), "(iTeam) - iTeam instance")

		// infos
		.def("getNumEffectInfos", &CyGlobalContext::getNumEffectInfos, "int () - Number of effect infos")
		.def("getEffectInfo", &CyGlobalContext::getEffectInfo, python::return_value_policy<python::reference_existing_object>(), "(int (EffectTypes) eEffectID) - CvInfo for EffectID")

		.def("getNumTerrainInfos", &CyGlobalContext::getNumTerrainInfos, "() - Total Terrain Infos XML\\Terrain\\CIV4TerrainInfos.xml")
		.def("getTerrainInfo", &CyGlobalContext::getTerrainInfo, python::return_value_policy<python::reference_existing_object>(), "(int (TerrainTypes) eTerrainID) - CvInfo for TerrainID")

		.def("getBonusClassInfo", &CyGlobalContext::getBonusClassInfo, python::return_value_policy<python::reference_existing_object>(), "(int (BonusClassTypes) eBonusClassID) - CvInfo for BonusID")

		.def("getNumBonusInfos", &CyGlobalContext::getNumBonusInfos, "() - Total Bonus Infos XML\\Terrain\\CIV4BonusInfos.xml")
		.def("getBonusInfo", &CyGlobalContext::getBonusInfo, python::return_value_policy<python::reference_existing_object>(), "(BonusID) - CvInfo for BonusID")

		.def("getNumFeatureInfos", &CyGlobalContext::getNumFeatureInfos, "() - Total Feature Infos XML\\Terrain\\CIV4FeatureInfos.xml")
		.def("getFeatureInfo", &CyGlobalContext::getFeatureInfo, python::return_value_policy<python::reference_existing_object>(), "(FeatureID) - CvInfo for FeatureID")

		.def("getNumUpkeepInfos", &CyGlobalContext::getNumUpkeepInfos, "int () - Number of upkeep infos")
		.def("getUpkeepInfo", &CyGlobalContext::getUpkeepInfo, python::return_value_policy<python::reference_existing_object>(), "(UpkeepInfoID) - CvInfo for upkeep info")

		.def("getNumCultureLevelInfos", &CyGlobalContext::getNumCultureLevelInfos, "int () - Number of culture level infos")
		.def("getCultureLevelInfo", &CyGlobalContext::getCultureLevelInfo, python::return_value_policy<python::reference_existing_object>(), "(CultureLevelID) - CvInfo for CultureLevelID")

		.def("getNumEraInfos", &CyGlobalContext::getNumEraInfos, "int () - Number of era infos")
		.def("getEraInfo", &CyGlobalContext::getEraInfo, python::return_value_policy<python::reference_existing_object>())

		.def("getNumWorldInfos", &CyGlobalContext::getNumWorldInfos, "int () - Number of world infos")
		.def("getWorldInfo", &CyGlobalContext::getWorldInfo, python::return_value_policy<python::reference_existing_object>(), "CvWorldInfo - (WorldTypeID)")

		.def("getNumClimateInfos", &CyGlobalContext::getNumClimateInfos, "int () - Number of climate infos")
		.def("getClimateInfo", &CyGlobalContext::getClimateInfo, python::return_value_policy<python::reference_existing_object>(), "CvClimateInfo - (ClimateTypeID)")

		.def("getNumSeaLevelInfos", &CyGlobalContext::getNumSeaLevelInfos, "int () - Number of seal level infos")
		.def("getSeaLevelInfo", &CyGlobalContext::getSeaLevelInfo, python::return_value_policy<python::reference_existing_object>(), "CvSeaLevelInfo - (SeaLevelTypeID)")

		.def("getNumPlayableCivilizationInfos", &CyGlobalContext::getNumPlayableCivilizationInfos, "() - Total # of Playable Civs")
		.def("getNumCivilizationInfos", &CyGlobalContext::getNumCivilizatonInfos, "() - Total Civilization Infos XML\\Civilizations\\CIV4CivilizationInfos.xml")
		.def("getCivilizationInfo", &CyGlobalContext::getCivilizationInfo, python::return_value_policy<python::reference_existing_object>(), "(CivilizationID) - CvInfo for CivilizationID")

		.def("getNumLeaderHeadInfos", &CyGlobalContext::getNumLeaderHeadInfos, "() - Total LeaderHead Infos XML\\Civilizations\\CIV4LeaderHeadInfos.xml")
		.def("getLeaderHeadInfo", &CyGlobalContext::getLeaderHeadInfo, python::return_value_policy<python::reference_existing_object>(), "(LeaderHeadID) - CvInfo for LeaderHeadID")

		.def("getNumTraitInfos", &CyGlobalContext::getNumTraitInfos, "() - Total Civilization Infos XML\\Civilizations\\CIV4TraitInfos.xml")
		.def("getTraitInfo", &CyGlobalContext::getTraitInfo, python::return_value_policy<python::reference_existing_object>(), "(TraitID) - CvInfo for TraitID")

		.def("getNumUnitInfos", &CyGlobalContext::getNumUnitInfos, "() - Total Unit Infos XML\\Units\\CIV4UnitInfos.xml")
		.def("getUnitInfo", &CyGlobalContext::getUnitInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitID) - CvInfo for UnitID")

		.def("getNumSpecialUnitInfos", &CyGlobalContext::getNumSpecialUnitInfos, "() - Total SpecialUnit Infos XML\\Units\\CIV4SpecialUnitInfos.xml")
		.def("getSpecialUnitInfo", &CyGlobalContext::getSpecialUnitInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitID) - CvInfo for UnitID")

		.def("getYieldInfo", &CyGlobalContext::getYieldInfo, python::return_value_policy<python::reference_existing_object>(), "(YieldID) - CvInfo for YieldID")

		.def("getCommerceInfo", &CyGlobalContext::getCommerceInfo, python::return_value_policy<python::reference_existing_object>(), "(CommerceID) - CvInfo for CommerceID")

		.def("getNumRouteInfos", &CyGlobalContext::getNumRouteInfos, "() - Total Route Infos XML\\Misc\\CIV4RouteInfos.xml")
		.def("getRouteInfo", &CyGlobalContext::getRouteInfo, python::return_value_policy<python::reference_existing_object>(), "(RouteID) - CvInfo for RouteID")

		.def("getNumImprovementInfos", &CyGlobalContext::getNumImprovementInfos, "() - Total Improvement Infos XML\\Terrain\\CIV4ImprovementInfos.xml")
		.def("getImprovementInfo", &CyGlobalContext::getImprovementInfo, python::return_value_policy<python::reference_existing_object>(), "(ImprovementID) - CvInfo for ImprovementID")

		.def("getNumGoodyInfos", &CyGlobalContext::getNumGoodyInfos, "() - Total Goody Infos XML\\GameInfo\\CIV4GoodyInfos.xml")
		.def("getGoodyInfo", &CyGlobalContext::getGoodyInfo, python::return_value_policy<python::reference_existing_object>(), "(GoodyID) - CvInfo for GoodyID")

		.def("getNumBuildInfos", &CyGlobalContext::getNumBuildInfos, "() - Total Build Infos XML\\Units\\CIV4BuildInfos.xml")
		.def("getBuildInfo", &CyGlobalContext::getBuildInfo, python::return_value_policy<python::reference_existing_object>(), "(BuildID) - CvInfo for BuildID")

		.def("getNumHandicapInfos", &CyGlobalContext::getNumHandicapInfos, "() - Total Handicap Infos XML\\GameInfo\\CIV4HandicapInfos.xml")
		.def("getHandicapInfo", &CyGlobalContext::getHandicapInfo, python::return_value_policy<python::reference_existing_object>(), "(HandicapID) - CvInfo for HandicapID")

		.def("getNumGameSpeedInfos", &CyGlobalContext::getNumGameSpeedInfos, "() - Total Game speed Infos XML\\GameInfo\\CIV4GameSpeedInfo.xml")
		.def("getGameSpeedInfo", &CyGlobalContext::getGameSpeedInfo, python::return_value_policy<python::reference_existing_object>(), "(GameSpeed Info) - CvInfo for GameSpeedID")

		.def("getNumBuildingInfos", &CyGlobalContext::getNumBuildingInfos, "() - Total Building Infos XML\\Buildings\\CIV4BuildingInfos.xml")
		.def("getBuildingInfo", &CyGlobalContext::getBuildingInfo, python::return_value_policy<python::reference_existing_object>(), "(BuildingID) - CvInfo for BuildingID")

		.def("getNumUnitCombatInfos", &CyGlobalContext::getNumUnitCombatInfos, "() - Total Unit Combat Infos XML\\Units\\CIV4UnitCombatInfos.xml")
		.def("getUnitCombatInfo", &CyGlobalContext::getUnitCombatInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitCombatID) - CvInfo for UnitCombatID")

		.def("getDomainInfo", &CyGlobalContext::getDomainInfo, python::return_value_policy<python::reference_existing_object>(), "(DomainID) - CvInfo for DomainID")

		.def("getNumActionInfos", &CyGlobalContext::getNumActionInfos, "() - Total Action Infos XML\\Units\\CIV4ActionInfos.xml")
		.def("getActionInfo", &CyGlobalContext::getActionInfo, python::return_value_policy<python::reference_existing_object>(), "(ActionID) - CvInfo for ActionID")

		.def("getNumPromotionLineInfos", &CyGlobalContext::getNumPromotionLineInfos, "() - Total Promotion Line Infos XML\\Units\\CIV4PromotionLineInfos.xml")
		.def("getPromotionLineInfo", &CyGlobalContext::getPromotionLineInfo, python::return_value_policy<python::reference_existing_object>(), "(PromotionLineID) - CvInfo for PromotionLineID")

		//.def("getNumIdeaClassInfos", &CyGlobalContext::getNumIdeaClassInfos, "() - Total Idea Class Infos XML\\GameInfo\\CIV4IdeaClassInfos.xml")
		//.def("getIdeaClassInfo", &CyGlobalContext::getIdeaClassInfo, python::return_value_policy<python::reference_existing_object>(), "(IdeaClassID) - CvInfo for IdeaClassID")

		//.def("getNumIdeaInfos", &CyGlobalContext::getNumIdeaInfos, "() - Total Idea Infos XML\\GameInfo\\CIV4IdeaInfos.xml")
		//.def("getIdeaInfo", &CyGlobalContext::getIdeaInfo, python::return_value_policy<python::reference_existing_object>(), "(IdeaID) - CvInfo for IdeaID")

		//.def("getNumInvisibleInfos", &CyGlobalContext::getNumInvisibleInfos, "() - Total Invisible Infos XML\\GameInfo\\CIV4InvisibleInfos.xml")
		//.def("getInvisibleInfo", &CyGlobalContext::getInvisibleInfo, python::return_value_policy<python::reference_existing_object>(), "(InvisibleID) - CvInfo for InvisibleID")

		//.def("getNumTraitOptionEditsInfos", &CyGlobalContext::getNumTraitOptionEditsInfos, "() - Total Trait Option Edits Infos XML\\Traits\\CIV4TraitOptionEditsInfos.xml")
		//.def("getTraitOptionEditsInfo", &CyGlobalContext::getTraitOptionEditsInfo, python::return_value_policy<python::reference_existing_object>(), "(TraitOptionEditsID) - CvInfo for TraitOptionEditsID")

		.def("setIsBug", &CyGlobalContext::setIsBug, "void () - init BUG on dll side")
	;
}
