#include "CvGameCoreDLL.h"
#include "CvInfos.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
//
// Python interface for info classes (formerly structs)
// These are simple enough to be exposed directly - no wrappers
//

void CyInfoPythonInterface2()
{
	OutputDebugString("Python Extension Module - CyInfoPythonInterface2\n");

	python::class_<CvRouteModelInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvRouteModelInfo", python::no_init)
	;

	python::class_<CvCivilizationInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvCivilizationInfo", python::no_init)

		.def("getArtStyleType", &CvCivilizationInfo::getArtStyleType, "int ()")
		.def("getNumCityNames", &CvCivilizationInfo::getNumCityNames, "int ()")
		.def("getNumLeaders", &CvCivilizationInfo::getNumLeaders, "int ()")

		.def("isAIPlayable", &CvCivilizationInfo::isAIPlayable, "bool ()")
		.def("isPlayable", &CvCivilizationInfo::isPlayable, "bool ()")

		.def("getShortDescription", &CvCivilizationInfo::pyGetShortDescription, "wstring ()")
		.def("getAdjective", &CvCivilizationInfo::pyGetAdjective, "wstring ()")
		.def("getArtDefineTag", &CvCivilizationInfo::getArtDefineTag, "string ()")
		.def("getButton", &CvCivilizationInfo::getButton, "string ()")

		.def("getDerivativeCiv", &CvCivilizationInfo::getDerivativeCiv, "int ()")

		// Arrays

		.def("isLeaders", &CvCivilizationInfo::isLeaders, "bool (int i)")
		.def("isCivilizationBuilding", &CvCivilizationInfo::isCivilizationBuilding, "bool (int i)")
		.def("isCivilizationFreeTechs", &CvCivilizationInfo::isCivilizationFreeTechs, "bool (int i)")

		.def("getCityNames", &CvCivilizationInfo::getCityNames, "string (int i)")
	;

	python::class_<CvVictoryInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvVictoryInfo", python::no_init)

		.def("getLandPercent", &CvVictoryInfo::getLandPercent, "int ()")
		.def("getMinLandPercent", &CvVictoryInfo::getMinLandPercent, "int ()")
		.def("getReligionPercent", &CvVictoryInfo::getReligionPercent, "int ()")
		.def("getCityCulture", &CvVictoryInfo::getCityCulture, "int ()")
		.def("getNumCultureCities", &CvVictoryInfo::getNumCultureCities, "int ()")
		.def("getTotalCultureRatio", &CvVictoryInfo::getTotalCultureRatio, "int ()")

		.def("isTotalVictory", &CvVictoryInfo::isTotalVictory, "bool ()")
		.def("isTargetScore", &CvVictoryInfo::isTargetScore, "bool ()")
		.def("isEndScore", &CvVictoryInfo::isEndScore, "bool ()")
		.def("isConquest", &CvVictoryInfo::isConquest, "bool ()")
		.def("isDiploVote", &CvVictoryInfo::isDiploVote, "bool ()")
		.def("isPermanent", &CvVictoryInfo::isPermanent, "bool ()")

		.def("getMovie", &CvVictoryInfo::getMovie, "string ()")
	;

	python::class_<CvHurryInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvHurryInfo", python::no_init)

		.def("getGoldPerProduction", &CvHurryInfo::getGoldPerProduction, "int ()")
		.def("getProductionPerPopulation", &CvHurryInfo::getProductionPerPopulation, "int ()")

		.def("isAnger", &CvHurryInfo::isAnger, "bool ()")
	;

	python::class_<CvHandicapInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvHandicapInfo", python::no_init)

		.def("getStartingGold", &CvHandicapInfo::getStartingGold, "int ()")
		.def("getTrainPercent", &CvHandicapInfo::getTrainPercent, "int ()")
		.def("getConstructPercent", &CvHandicapInfo::getConstructPercent, "int ()")
		.def("getResearchPercent", &CvHandicapInfo::getResearchPercent, "int ()")
		.def("getCorporationMaintenancePercent", &CvHandicapInfo::getCorporationMaintenancePercent, "int ()")
		.def("getCivicUpkeepPercent", &CvHandicapInfo::getCivicUpkeepPercent, "int ()")
		.def("getHealthBonus", &CvHandicapInfo::getHealthBonus, "int ()")

		.def("getStartingDefenseUnits", &CvHandicapInfo::getStartingDefenseUnits, "int ()")
		.def("getStartingWorkerUnits", &CvHandicapInfo::getStartingWorkerUnits, "int ()")
		.def("getStartingExploreUnits", &CvHandicapInfo::getStartingExploreUnits, "int ()")
		.def("getBarbarianInitialDefenders", &CvHandicapInfo::getBarbarianInitialDefenders, "int ()")
		.def("getAIUnitUpgradePercent", &CvHandicapInfo::getAIUnitUpgradePercent, "int ()")
		.def("getAIPerEraModifier", &CvHandicapInfo::getAIPerEraModifier, "int ()")
		.def("getRevolutionIndexPercent", &CvHandicapInfo::getRevolutionIndexPercent, "int ()")
	;

	python::class_<CvGameSpeedInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvGameSpeedInfo", python::no_init)

		.def("getSpeedPercent", &CvGameSpeedInfo::getSpeedPercent, "int ()")
		.def("getHammerCostPercent", &CvGameSpeedInfo::getHammerCostPercent, "int ()")

		.def("getNumTurnIncrements", &CvGameSpeedInfo::getNumTurnIncrements, "int ()")
		.def("getGameTurnInfo", &CvGameSpeedInfo::getGameTurnInfo, python::return_value_policy<python::reference_existing_object>(), "GameTurnInfo ()")
		.def("getDateIncrement", &CvGameSpeedInfo::getDateIncrement, python::return_value_policy<python::reference_existing_object>(), "CvDateIncrement ()")
	;

	python::class_<CvTurnTimerInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvTurnTimerInfo", python::no_init)

	;

	python::class_<CvBuildInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvBuildInfo", python::no_init)

		.def("getTime", &CvBuildInfo::getTime, "int ()")
		.def("getCost", &CvBuildInfo::getCost, "int ()")
		.def("getTechPrereq", &CvBuildInfo::getTechPrereq, "int ()")
		.def("getImprovement", &CvBuildInfo::getImprovement, "int ()")
		.def("getRoute", &CvBuildInfo::getRoute, "int ()")
		.def("getMissionType", &CvBuildInfo::getMissionType, "int ()")

		.def("isKill", &CvBuildInfo::isKill, "bool ()")
		.def("setDisabled", &CvBuildInfo::setDisabled, "bool ()")

		// Arrays
		.def("getFeatureTech", &CvBuildInfo::getFeatureTech, "int (int i)")
		.def("getFeatureTime", &CvBuildInfo::getFeatureTime, "int (int i)")
		.def("getFeatureProduction", &CvBuildInfo::getFeatureProduction, "int (int i)")

		.def("isFeatureRemove", &CvBuildInfo::isFeatureRemove, "bool (int i)")
	;

	python::class_<CvGoodyInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvGoodyInfo", python::no_init)

		.def("getGold", &CvGoodyInfo::getGold, "int ()")
		.def("getExperience", &CvGoodyInfo::getExperience, "int ()")

		.def("isTech", &CvGoodyInfo::isTech, "bool ()")

		.def("getSound", &CvGoodyInfo::getSound, "string ()")
	;

	python::class_<CvRouteInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvRouteInfo", python::no_init)

		.def("getValue", &CvRouteInfo::getValue, "int ()")
		.def("getMovementCost", &CvRouteInfo::getMovementCost, "int ()")
		.def("getFlatMovementCost", &CvRouteInfo::getFlatMovementCost, "int ()")
		.def("getPrereqBonus", &CvRouteInfo::getPrereqBonus, "int ()")

		// Arrays
		.def("getYieldChange", &CvRouteInfo::getYieldChange, "int (int i)")
		.def("getTechMovementChange", &CvRouteInfo::getTechMovementChange, "int (int i)")
		.def("getPrereqOrBonuses", &CvRouteInfo::getPrereqOrBonuses, python::return_value_policy<python::reference_existing_object>())
	;

	python::class_<CvImprovementBonusInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvImprovementBonusInfo", python::no_init)
		.def("getYieldChange", &CvImprovementBonusInfo::getYieldChange, "int (int i)")
	;

	python::class_<CvImprovementInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvImprovementInfo", python::no_init)

		.def("getTilesPerGoody", &CvImprovementInfo::getTilesPerGoody, "int ()")
		.def("getGoodyUniqueRange", &CvImprovementInfo::getGoodyUniqueRange, "int ()")
		.def("getUpgradeTime", &CvImprovementInfo::getUpgradeTime, "int ()")
		.def("getAirBombDefense", &CvImprovementInfo::getAirBombDefense, "int ()")
		.def("getDefenseModifier", &CvImprovementInfo::getDefenseModifier, "int ()")
		.def("getHappiness", &CvImprovementInfo::getHappiness, "int ()")
		.def("getPillageGold", &CvImprovementInfo::getPillageGold, "int ()")
		.def("getImprovementPillage", &CvImprovementInfo::getImprovementPillage, "int ()")
		.def("getImprovementUpgrade", &CvImprovementInfo::getImprovementUpgrade, "int ()")
		.def("getNumAlternativeImprovementUpgradeTypes", &CvImprovementInfo::getNumAlternativeImprovementUpgradeTypes, "int ()")
		.def("getPrereqTech", &CvImprovementInfo::getPrereqTech, "int ()")

		.def("isActsAsCity", &CvImprovementInfo::isActsAsCity, "bool ()")
		.def("isHillsMakesValid", &CvImprovementInfo::isHillsMakesValid, "bool ()")
		.def("isPeakMakesValid", &CvImprovementInfo::isPeakMakesValid, "bool ()")
		.def("isNoFreshWater", &CvImprovementInfo::isNoFreshWater, "bool ()")
		.def("isRequiresFlatlands", &CvImprovementInfo::isRequiresFlatlands, "bool ()")
		.def("isRequiresRiverSide", &CvImprovementInfo::isRequiresRiverSide, "bool ()")
		.def("isRequiresIrrigation", &CvImprovementInfo::isRequiresIrrigation, "bool ()")
		.def("isRequiresFeature", &CvImprovementInfo::isRequiresFeature, "bool ()")
		.def("isPeakImprovement", &CvImprovementInfo::isPeakImprovement, "bool ()")
		.def("isWaterImprovement", &CvImprovementInfo::isWaterImprovement, "bool ()")
		.def("isGoody", &CvImprovementInfo::isGoody, "bool ()")
		.def("isOutsideBorders", &CvImprovementInfo::isOutsideBorders, "bool ()")
		.def("getArtDefineTag", &CvImprovementInfo::getArtDefineTag, "string ()")

		// Arrays
		.def("getPrereqNatureYield", &CvImprovementInfo::getPrereqNatureYield, "int (int i)")
		.def("getYieldChange", &CvImprovementInfo::getYieldChange, "int (int i)")
		.def("getRiverSideYieldChange", &CvImprovementInfo::getRiverSideYieldChange, "int (int i)")
		.def("getIrrigatedYieldChange", &CvImprovementInfo::getIrrigatedYieldChange, "int (int i)")
		.def("getAlternativeImprovementUpgradeType", &CvImprovementInfo::getAlternativeImprovementUpgradeType, "int (int i)")

		.def("getTerrainMakesValid", &CvImprovementInfo::getTerrainMakesValid, "bool (int i)")
		.def("getFeatureMakesValid", &CvImprovementInfo::getFeatureMakesValid, "bool (int i)")
		.def("isAlternativeImprovementUpgradeType", &CvImprovementInfo::isAlternativeImprovementUpgradeType, "bool (int i)")

		.def("getImprovementBonusYield", &CvImprovementInfo::getImprovementBonusYield, "int (int i, int j)")
		.def("isImprovementBonusMakesValid", &CvImprovementInfo::isImprovementBonusMakesValid, "bool (int i)")
		.def("isImprovementBonusTrade", &CvImprovementInfo::isImprovementBonusTrade, "bool (int i)")
		.def("getImprovementBonusDiscoverRand", &CvImprovementInfo::getImprovementBonusDiscoverRand, "int (int i)")

		.def("getTechYieldChanges", &CvImprovementInfo::getTechYieldChanges, "int (int i, int j)")
		.def("getRouteYieldChanges", &CvImprovementInfo::getRouteYieldChanges, "int (int i, int j)")
	;

	python::class_<CvBonusClassInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvBonusClassInfo", python::no_init)
		.def("getUniqueRange", &CvBonusClassInfo::getUniqueRange)
	;

	python::class_<CvBonusInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvBonusInfo", python::no_init)

		.def("getChar", &CvBonusInfo::getChar, "int ()")
		.def("getTechReveal", &CvBonusInfo::getTechReveal, "int ()")
		.def("getTechCityTrade", &CvBonusInfo::getTechCityTrade, "int ()")
		.def("getTechObsolete", &CvBonusInfo::getTechObsolete, "int ()")
		.def("getHealth", &CvBonusInfo::getHealth, "int ()")
		.def("getHappiness", &CvBonusInfo::getHappiness, "int ()")
		.def("getMinAreaSize", &CvBonusInfo::getMinAreaSize, "int ()")
		.def("getMinLatitude", &CvBonusInfo::getMinLatitude, "int ()")
		.def("getMaxLatitude", &CvBonusInfo::getMaxLatitude, "int ()")
		.def("getPlacementOrder", &CvBonusInfo::getPlacementOrder, "int ()")
		.def("getConstAppearance", &CvBonusInfo::getConstAppearance, "int ()")
		.def("getRandAppearance1", &CvBonusInfo::getRandAppearance1, "int ()")
		.def("getRandAppearance2", &CvBonusInfo::getRandAppearance2, "int ()")
		.def("getRandAppearance3", &CvBonusInfo::getRandAppearance3, "int ()")
		.def("getRandAppearance4", &CvBonusInfo::getRandAppearance4, "int ()")
		.def("getPercentPerPlayer", &CvBonusInfo::getPercentPerPlayer, "int ()")
		.def("getTilesPer", &CvBonusInfo::getTilesPer, "int ()")
		.def("getMinLandPercent", &CvBonusInfo::getMinLandPercent, "int ()")
		.def("getUniqueRange", &CvBonusInfo::getUniqueRange, "int ()")
		.def("getGroupRange", &CvBonusInfo::getGroupRange, "int ()")
		.def("getGroupRand", &CvBonusInfo::getGroupRand, "int ()")
		.def("getBonusClassType", &CvBonusInfo::getBonusClassType, "int ()")

		.def("isOneArea", &CvBonusInfo::isOneArea, "bool ()")
		.def("isHills", &CvBonusInfo::isHills, "bool ()")
		.def("isPeaks", &CvBonusInfo::isPeaks, "bool ()")
		.def("isFlatlands", &CvBonusInfo::isFlatlands, "bool ()")
		.def("isBonusCoastalOnly", &CvBonusInfo::isBonusCoastalOnly, "bool ()")
		.def("isNoRiverSide", &CvBonusInfo::isNoRiverSide, "bool ()")
		.def("isNormalize", &CvBonusInfo::isNormalize, "bool ()")

		.def("getArtDefineTag", &CvBonusInfo::getArtDefineTag, "string ()")

		// Arrays
		.def("getYieldChange", &CvBonusInfo::getYieldChange, "int (int i)")

		.def("isTerrain", &CvBonusInfo::isTerrain, "bool (int i)")
		.def("isFeature", &CvBonusInfo::isFeature, "bool (int i)")
		.def("isFeatureTerrain", &CvBonusInfo::isFeatureTerrain, "bool (int i)")

		.def("getButton", &CvBonusInfo::getButton, "string ()")
	;

	python::class_<CvFeatureInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvFeatureInfo", python::no_init)

		.def("getMovementCost", &CvFeatureInfo::getMovementCost, "int ()")
		.def("getAppearanceProbability", &CvFeatureInfo::getAppearanceProbability, "int ()")
		.def("getDisappearanceProbability", &CvFeatureInfo::getDisappearanceProbability, "int ()")
		.def("getGrowthProbability", &CvFeatureInfo::getGrowthProbability, "int ()")
		.def("getDefenseModifier", &CvFeatureInfo::getDefenseModifier, "int ()")
		.def("getTurnDamage", &CvFeatureInfo::getTurnDamage, "int ()")
		.def("getWarmingDefense", &CvFeatureInfo::getWarmingDefense, "int ()")
		.def("getNumVarieties", &CvFeatureInfo::getNumVarieties, "int ()")

		.def("isNoRiver", &CvFeatureInfo::isNoRiver, "bool ()")
		.def("isRequiresFlatlands", &CvFeatureInfo::isRequiresFlatlands, "bool ()")
		.def("isRequiresRiver", &CvFeatureInfo::isRequiresRiver, "bool ()")
		.def("isImpassable", &CvFeatureInfo::isImpassable, "bool ()")
		.def("isNoCity", &CvFeatureInfo::isNoCity, "bool ()")
		.def("isNukeImmune", &CvFeatureInfo::isNukeImmune, "bool ()")

		// Arrays
		.def("getYieldChange", &CvFeatureInfo::getYieldChange, "int (int i)")
		.def("getRiverYieldChange", &CvFeatureInfo::getRiverYieldChange, "int (int i)")

		.def("isTerrain", &CvFeatureInfo::isTerrain, "bool (int i)")
	;

	python::class_<CvCommerceInfo, python::bases<CvInfoBase>, boost::noncopyable>("CvCommerceInfo", python::no_init)
		.def("getChar", &CvCommerceInfo::getChar, "int ()")
	;
}