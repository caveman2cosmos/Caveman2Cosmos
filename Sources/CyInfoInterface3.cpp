#include "CvGameCoreDLL.h"

//
// Python interface for info classes (formerly structs)
// These are simple enough to be exposed directly - no wrappers
//

void CyInfoPythonInterface3()
{
	OutputDebugString("Python Extension Module - CyInfoPythonInterface3\n");

	python::class_<CvYieldInfo, python::bases<CvInfoBase> >("CvYieldInfo", python::no_init)

		.def("getChar", &CvYieldInfo::getChar, "int ()")
		.def("getColorType", &CvYieldInfo::getColorType, "int ()")
	;


	python::class_<CvTerrainInfo, python::bases<CvInfoBase> >("CvTerrainInfo", python::no_init)

		.def("getMovementCost", &CvTerrainInfo::getMovementCost, "int ()")
		.def("getDefenseModifier", &CvTerrainInfo::getDefenseModifier, "int ()")

		.def("isWaterTerrain", &CvTerrainInfo::isWaterTerrain, "bool ()")
		.def("isImpassable", &CvTerrainInfo::isImpassable, "bool ()")
		.def("isFound", &CvTerrainInfo::isFound, "bool ()")

		// Arrays
		.def("getYield", &CvTerrainInfo::getYield, "int (int i)")
	;


	python::class_<CvInterfaceModeInfo, python::bases<CvInfoBase> >("CvInterfaceModeInfo", python::no_init)

		.def("getMissionType", &CvInterfaceModeInfo::getMissionType, "int ()")

		.def("getVisible", &CvInterfaceModeInfo::getVisible, "bool ()")
		.def("getGotoPlot", &CvInterfaceModeInfo::getGotoPlot, "bool ()")
		.def("getHighlightPlot", &CvInterfaceModeInfo::getHighlightPlot, "bool ()")
	;


	python::class_<CvLeaderHeadInfo, python::bases<CvInfoBase> >("CvLeaderHeadInfo", python::no_init)

		.def("isNPC", &CvLeaderHeadInfo::isNPC, "bool ()")
		.def("getWonderConstructRand", &CvLeaderHeadInfo::getWonderConstructRand, "int ()")
		.def("getBaseAttitude", &CvLeaderHeadInfo::getBaseAttitude, "int ()")
		.def("getWarmongerRespect", &CvLeaderHeadInfo::getWarmongerRespect, "int ()")
		.def("getMaxWarRand", &CvLeaderHeadInfo::getMaxWarRand, "int ()")
		.def("getMaxWarNearbyPowerRatio", &CvLeaderHeadInfo::getMaxWarNearbyPowerRatio, "int ()")
		.def("getMaxWarDistantPowerRatio", &CvLeaderHeadInfo::getMaxWarDistantPowerRatio, "int ()")
		.def("getMaxWarMinAdjacentLandPercent", &CvLeaderHeadInfo::getMaxWarMinAdjacentLandPercent, "int ()")
		.def("getLimitedWarRand", &CvLeaderHeadInfo::getLimitedWarRand, "int ()")
		.def("getLimitedWarPowerRatio", &CvLeaderHeadInfo::getLimitedWarPowerRatio, "int ()")
		.def("getDogpileWarRand", &CvLeaderHeadInfo::getDogpileWarRand, "int ()")
		.def("getRazeCityProb", &CvLeaderHeadInfo::getRazeCityProb, "int ()")
		.def("getFavoriteCivic", &CvLeaderHeadInfo::getFavoriteCivic, "int ()")
		.def("getFavoriteReligion", &CvLeaderHeadInfo::getFavoriteReligion, "int ()")
		.def("getArtDefineTag", &CvLeaderHeadInfo::getArtDefineTag, "string ()")

		// Arrays
		.def("hasTrait", &CvLeaderHeadInfo::hasTrait, "bool (int i)")
		.def("getFlavorValue", &CvLeaderHeadInfo::getFlavorValue, "int (int i)")
		.def("getDiploPeaceMusicScriptIds", &CvLeaderHeadInfo::getDiploPeaceMusicScriptIds, "int (int i)")

		// Other
		.def("getLeaderHead", &CvLeaderHeadInfo::getLeaderHead, "string ()")
		.def("getButton", &CvLeaderHeadInfo::getButton, "string ()")
	;

	// CvProcessInfos
	python::class_<CvProcessInfo, python::bases<CvInfoBase> >("CvProcessInfo", python::no_init)

		.def("getTechPrereq", &CvProcessInfo::getTechPrereq, "int ()")
		// Arrays
		.def("getProductionToCommerceModifier", &CvProcessInfo::getProductionToCommerceModifier, "int (int i)")
	;


	python::class_<CvVoteInfo, python::bases<CvInfoBase> >("CvVoteInfo", python::no_init)

		.def("getTradeRoutes", &CvVoteInfo::getTradeRoutes, "int ()")

		.def("isSecretaryGeneral", &CvVoteInfo::isSecretaryGeneral, "bool ()")
		.def("isVictory", &CvVoteInfo::isVictory, "bool ()")
		.def("isDefensivePact", &CvVoteInfo::isDefensivePact, "bool ()")
		.def("isOpenBorders", &CvVoteInfo::isOpenBorders, "bool ()")
		.def("isForcePeace", &CvVoteInfo::isForcePeace, "bool ()")

		// Arrays
		.def("isVoteSourceType", &CvVoteInfo::isVoteSourceType, "bool (int i)")
	;


	python::class_<CvProjectInfo, python::bases<CvInfoBase> >("CvProjectInfo", python::no_init)

		.def("getVictoryPrereq", &CvProjectInfo::getVictoryPrereq, "int ()")
		.def("getTechPrereq", &CvProjectInfo::getTechPrereq, "int ()")
		.def("getMaxGlobalInstances", &CvProjectInfo::getMaxGlobalInstances, "int ()")
		.def("getMaxTeamInstances", &CvProjectInfo::getMaxTeamInstances, "int ()")
		.def("getProductionCost", &CvProjectInfo::getProductionCost, "int ()")
		.def("getNukeInterception", &CvProjectInfo::getNukeInterception, "int ()")

		.def("isSpaceship", &CvProjectInfo::isSpaceship, "bool ()")
		.def("isAllowsNukes", &CvProjectInfo::isAllowsNukes, "bool ()")

		.def("getMovieArtDef", &CvProjectInfo::getMovieArtDef, "string ()")

		// Arrays
		.def("getBonusProductionModifier", &CvProjectInfo::getBonusProductionModifier, "int (int i)")
		.def("getVictoryThreshold", &CvProjectInfo::getVictoryThreshold, "int (int i)")
		.def("getVictoryMinThreshold", &CvProjectInfo::getVictoryMinThreshold, "int (int i)")
		.def("getVictoryDelayPercent", &CvProjectInfo::getVictoryDelayPercent, "int ()")
		.def("getProjectsNeeded", &CvProjectInfo::getProjectsNeeded, "int (int i)")
	;


	python::class_<CvReligionInfo, python::bases<CvInfoBase> >("CvReligionInfo", python::no_init)

		.def("getChar", &CvReligionInfo::getChar, "int ()")
		.def("getHolyCityChar", &CvReligionInfo::getHolyCityChar, "int ()")
		.def("getTechPrereq", &CvReligionInfo::getTechPrereq, "int ()")
		.def("getMissionType", &CvReligionInfo::getMissionType, "int ()")

		.def("getTechButton", &CvReligionInfo::getTechButton, "string ()")
		.def("getGenericTechButton", &CvReligionInfo::getGenericTechButton, "string ()")
		.def("getMovieFile", &CvReligionInfo::getMovieFile, "string ()")
		.def("getMovieSound", &CvReligionInfo::getMovieSound, "string ()")
		.def("getSound", &CvReligionInfo::getSound, "string ()")
		.def("getButtonDisabled", &CvReligionInfo::getButtonDisabled, "string ()")
		.def("getAdjectiveKey", &CvReligionInfo::pyGetAdjectiveKey, "wstring ()")
		// Arrays
		.def("getGlobalReligionCommerce", &CvReligionInfo::getGlobalReligionCommerce, "int (int i)")
	;


	python::class_<CvCorporationInfo, python::bases<CvInfoBase> >("CvCorporationInfo", python::no_init)

		.def("getChar", &CvCorporationInfo::getChar, "int ()")
		.def("getHeadquarterChar", &CvCorporationInfo::getHeadquarterChar, "int ()")
		.def("getTechPrereq", &CvCorporationInfo::getTechPrereq, "int ()")
		.def("getObsoleteTech", &CvCorporationInfo::getObsoleteTech, "int ()")

		.def("getMaintenance", &CvCorporationInfo::getMaintenance, "int ()")
		.def("getMissionType", &CvCorporationInfo::getMissionType, "int ()")

		.def("getMovieFile", &CvCorporationInfo::getMovieFile, "string ()")
		.def("getMovieSound", &CvCorporationInfo::getMovieSound, "string ()")
		.def("getSound", &CvCorporationInfo::getSound, "string ()")

		// Arrays
		.def("getPrereqBonuses", &CvCorporationInfo::getPrereqBonuses, python::return_value_policy<python::reference_existing_object>())
		.def("getCommerceProduced", &CvCorporationInfo::getCommerceProduced, "int (int i)")
		.def("getYieldProduced", &CvCorporationInfo::getYieldProduced, "int (int i)")
	;


	python::class_<CvTraitInfo, python::bases<CvInfoBase> >("CvTraitInfo", python::no_init)

		.def("getHealth", &CvTraitInfo::getHealth, "int ()")
		.def("getHappiness", &CvTraitInfo::getHappiness, "int ()")
		.def("getLevelExperienceModifier", &CvTraitInfo::getLevelExperienceModifier, "int ()")
		.def("getGreatPeopleRateModifier", &CvTraitInfo::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CvTraitInfo::getGreatGeneralRateModifier, "int ()")
		.def("getNumBuildingProductionModifiers", &CvTraitInfo::getNumBuildingProductionModifiers, "int ()")

		.def("getRevIdxLocal", &CvTraitInfo::getRevIdxLocal, "int ()")
		.def("getRevIdxNational", &CvTraitInfo::getRevIdxNational, "int ()")
		.def("getRevIdxDistanceModifier", &CvTraitInfo::getRevIdxDistanceModifier, "int ()")
		.def("getRevIdxHolyCityGood", &CvTraitInfo::getRevIdxHolyCityGood, "int ()")
		.def("getRevIdxHolyCityBad", &CvTraitInfo::getRevIdxHolyCityBad, "int ()")
		.def("getRevIdxNationalityMod", &CvTraitInfo::getRevIdxNationalityMod, "float ()")
		.def("getRevIdxBadReligionMod", &CvTraitInfo::getRevIdxBadReligionMod, "float ()")
		.def("getRevIdxGoodReligionMod", &CvTraitInfo::getRevIdxGoodReligionMod, "float ()")

		.def("getShortDescription", &CvTraitInfo::getShortDescription, "int (int i)")
		.def("getCommerceChange", &CvTraitInfo::getCommerceChange, "int (int i)")
		.def("getCommerceModifier", &CvTraitInfo::getCommerceModifier, "int (int i)")
		.def("getBuildingProductionModifier", &CvTraitInfo::getBuildingProductionModifier)
		.def("getBuildingHappinessModifiers", &CvTraitInfo::getBuildingHappinessModifiers, python::return_value_policy<python::reference_existing_object>())

		.def("getImprovementYieldChange", &CvTraitInfo::getImprovementYieldChange, "int (int i, int j)")
	;

	python::class_<CvWorldInfo, python::bases<CvInfoBase> >("CvWorldInfo", python::no_init)

		.def("getDefaultPlayers", &CvWorldInfo::getDefaultPlayers, "int ()")
		.def("getTargetNumCities", &CvWorldInfo::getTargetNumCities, "int ()")
		.def("getBuildingPrereqModifier", &CvWorldInfo::getBuildingPrereqModifier, "int ()")
		.def("getWarWearinessModifier", &CvWorldInfo::getWarWearinessModifier, "int ()")
		.def("getGridWidth", &CvWorldInfo::getGridWidth, "int ()")
		.def("getGridHeight", &CvWorldInfo::getGridHeight, "int ()")
		.def("getTerrainGrainChange", &CvWorldInfo::getTerrainGrainChange, "int ()")
		.def("getFeatureGrainChange", &CvWorldInfo::getFeatureGrainChange, "int ()")
		.def("getResearchPercent", &CvWorldInfo::getResearchPercent, "int ()")
		.def("getCorporationMaintenancePercent", &CvWorldInfo::getCorporationMaintenancePercent, "int ()")
		.def("getOceanMinAreaSize", &CvWorldInfo::getOceanMinAreaSize, "int ()")
	;

	python::class_<CvMapInfo, python::bases<CvInfoBase> >("CvMapInfo", python::no_init)

		.def("getGridWidth", &CvMapInfo::getGridWidth, "int ()")
		.def("getGridHeight", &CvMapInfo::getGridHeight, "int ()")
		.def("getWrapX", &CvMapInfo::getWrapX, "int ()")
		.def("getWrapY", &CvMapInfo::getWrapY, "int ()")
		.def("getMapScript", &CvMapInfo::getMapScript, "string ()")
	;

	python::class_<CvClimateInfo, python::bases<CvInfoBase> >("CvClimateInfo", python::no_init)

		.def("getDesertPercentChange", &CvClimateInfo::getDesertPercentChange, "int ()")
		.def("getJungleLatitude", &CvClimateInfo::getJungleLatitude, "int ()")
		.def("getHillRange", &CvClimateInfo::getHillRange, "int ()")
		.def("getPeakPercent", &CvClimateInfo::getPeakPercent, "int ()")

		.def("getSnowLatitudeChange", &CvClimateInfo::getSnowLatitudeChange, "float ()")
		.def("getTundraLatitudeChange", &CvClimateInfo::getTundraLatitudeChange, "float ()")
		.def("getGrassLatitudeChange", &CvClimateInfo::getGrassLatitudeChange, "float ()")
		.def("getDesertBottomLatitudeChange", &CvClimateInfo::getDesertBottomLatitudeChange, "float ()")
		.def("getDesertTopLatitudeChange", &CvClimateInfo::getDesertTopLatitudeChange, "float ()")
		.def("getRandIceLatitude", &CvClimateInfo::getRandIceLatitude, "float ()")
	;

	python::class_<CvSeaLevelInfo, python::bases<CvInfoBase> >("CvSeaLevelInfo", python::no_init)

		.def("getSeaLevelChange", &CvSeaLevelInfo::getSeaLevelChange, "int ()")
	;


	python::class_<CvAssetInfoBase>("CvAssetInfoBase", python::no_init)

		.def("setPath", &CvAssetInfoBase::setPath, "void (string)")
		.def("getPath", &CvAssetInfoBase::getPath, "string ()")
	;


	python::class_<CvArtInfoAsset, python::bases<CvAssetInfoBase> >("CvArtInfoAsset", python::no_init)

		.def("getButton", &CvArtInfoAsset::getButton, "string ()")
	;


	python::class_<CvArtInfoScalableAsset, python::bases<CvArtInfoAsset, CvScalableInfo> >("CvArtInfoScalableAsset", python::no_init);


	python::class_<CvArtInfoInterface, python::bases<CvArtInfoAsset> >("CvArtInfoInterface", python::no_init);


	python::class_<CvArtInfoMovie, python::bases<CvArtInfoAsset> >("CvArtInfoMovie", python::no_init);


	python::class_<CvArtInfoMisc, python::bases<CvArtInfoAsset> >("CvArtInfoMisc", python::no_init);


	python::class_<CvArtInfoUnit, python::bases<CvArtInfoScalableAsset> >("CvArtInfoUnit", python::no_init)

	;


	python::class_<CvArtInfoBuilding, python::bases<CvArtInfoScalableAsset> >("CvArtInfoBuilding", python::no_init)

	;


	python::class_<CvArtInfoCivilization, python::bases<CvArtInfoAsset> >("CvArtInfoCivilization", python::no_init)

		.def("isWhiteFlag", &CvArtInfoCivilization::isWhiteFlag, "bool ()")
	;

	python::class_<CvArtInfoLeaderhead, python::bases<CvArtInfoAsset> >("CvArtInfoLeaderhead", python::no_init);


	python::class_<CvArtInfoBonus, python::bases<CvArtInfoScalableAsset> >("CvArtInfoBonus", python::no_init);


	python::class_<CvArtInfoImprovement, python::bases<CvArtInfoScalableAsset> >("CvArtInfoImprovement", python::no_init)

	;


	python::class_<CvArtInfoTerrain, python::bases<CvArtInfoAsset> >("CvArtInfoTerrain", python::no_init);


	python::class_<CvArtInfoFeature, python::bases<CvArtInfoScalableAsset> >("CvArtInfoFeature", python::no_init)

	;


	python::class_<CvEmphasizeInfo, python::bases<CvInfoBase> >("CvEmphasizeInfo", python::no_init)


		// Arrays
		.def("getYieldChange", &CvEmphasizeInfo::getYieldChange, "int (int i)")
		.def("getCommerceChange", &CvEmphasizeInfo::getCommerceChange, "int (int i)")
	;


	python::class_<CvUpkeepInfo, python::bases<CvInfoBase> >("CvUpkeepInfo", python::no_init)

	;


	python::class_<CvCultureLevelInfo, python::bases<CvInfoBase> >("CvCultureLevelInfo", python::no_init)

		.def("getSpeedThreshold", &CvCultureLevelInfo::getSpeedThreshold, "int ()")
	;


	python::class_<CvEraInfo, python::bases<CvInfoBase> >("CvEraInfo", python::no_init)

		.def("getStartingUnitMultiplier", &CvEraInfo::getStartingUnitMultiplier, "int () -")
		.def("getStartingDefenseUnits", &CvEraInfo::getStartingDefenseUnits, "int () -")
		.def("getStartingWorkerUnits", &CvEraInfo::getStartingWorkerUnits, "int () -")
		.def("getStartingExploreUnits", &CvEraInfo::getStartingExploreUnits, "int () -")
		.def("getStartingGold", &CvEraInfo::getStartingGold, "int () -")
		.def("getFreePopulation", &CvEraInfo::getFreePopulation, "int () -")
		.def("getStartPercent", &CvEraInfo::getStartPercent, "int () -")
		.def("getGrowthPercent", &CvEraInfo::getGrowthPercent, "int () -")
		.def("getTrainPercent", &CvEraInfo::getTrainPercent, "int () -")
		.def("getConstructPercent", &CvEraInfo::getConstructPercent, "int () -")
		.def("getCreatePercent", &CvEraInfo::getCreatePercent, "int () -")
		.def("getResearchPercent", &CvEraInfo::getResearchPercent, "int () -")
		.def("getBuildPercent", &CvEraInfo::getBuildPercent, "int () -")
		.def("getImprovementPercent", &CvEraInfo::getImprovementPercent, "int () -")
		.def("getGreatPeoplePercent", &CvEraInfo::getGreatPeoplePercent, "int () -")
		.def("getAnarchyPercent", &CvEraInfo::getAnarchyPercent, "int () -")
		.def("getEventChancePerTurn", &CvEraInfo::getEventChancePerTurn, "int () -")
		.def("getNumSoundtracks", &CvEraInfo::getNumSoundtracks, "int () -")

		.def("isNoGoodies", &CvEraInfo::isNoGoodies, "bool () -")
		.def("isNoAnimals", &CvEraInfo::isNoAnimals, "bool () -")
		.def("isNoBarbUnits", &CvEraInfo::isNoBarbUnits, "bool () -")
		.def("isNoBarbCities", &CvEraInfo::isNoBarbCities, "bool () -")

		// Arrays
	;


	python::class_<CvColorInfo, python::bases<CvInfoBase> >("CvColorInfo", python::no_init)

		.def("getColor", &CvColorInfo::getColor,  python::return_value_policy<python::reference_existing_object>())
	;


	python::class_<CvPlayerColorInfo, python::bases<CvInfoBase> >("CvPlayerColorInfo", python::no_init)

		.def("getColorTypePrimary", &CvPlayerColorInfo::getColorTypePrimary, "int ()")
	;


	python::class_<CvGameText, python::bases<CvInfoBase> >("CvGameText")

		.def("getText", &CvGameText::pyGetText, "wstring ()")
		.def("setText", &CvGameText::setText, "void (wstring)")
		.def("getNumLanguages", &CvGameText::getNumLanguages, "int ()")
	;


	python::class_<CvDiplomacyTextInfo, python::bases<CvInfoBase> >("CvDiplomacyTextInfo", python::no_init)

		.def("getNumResponses", &CvDiplomacyTextInfo::getNumResponses, "int ()")

		.def("getCivilizationTypes", &CvDiplomacyTextInfo::getCivilizationTypes, "bool (int i, int j)")
		.def("getLeaderHeadTypes", &CvDiplomacyTextInfo::getLeaderHeadTypes, "bool (int i, int j)")
		.def("getAttitudeTypes", &CvDiplomacyTextInfo::getAttitudeTypes, "bool (int i, int j)")
		.def("getDiplomacyPowerTypes", &CvDiplomacyTextInfo::getDiplomacyPowerTypes, "bool (int i, int j)")

		.def("getNumDiplomacyText", &CvDiplomacyTextInfo::getNumDiplomacyText, "int (int i)")
		.def("getDiplomacyText", &CvDiplomacyTextInfo::getDiplomacyText, "string (int i, int j)")
	;


	python::class_<CvDiplomacyInfo, python::bases<CvInfoBase> >("CvDiplomacyInfo", python::no_init)

		.def("getNumResponses", &CvDiplomacyInfo::getNumResponses, "int ()")

		.def("getCivilizationTypes", &CvDiplomacyInfo::getCivilizationTypes, "bool (int i, int j)")
		.def("getLeaderHeadTypes", &CvDiplomacyInfo::getLeaderHeadTypes, "bool (int i, int j)")
		.def("getAttitudeTypes", &CvDiplomacyInfo::getAttitudeTypes, "bool (int i, int j)")
		.def("getDiplomacyPowerTypes", &CvDiplomacyInfo::getDiplomacyPowerTypes, "bool (int i, int j)")

		.def("getNumDiplomacyText", &CvDiplomacyInfo::getNumDiplomacyText, "int (int i)")
		.def("getDiplomacyText", &CvDiplomacyInfo::getDiplomacyText, "string (int i, int j)")
	;


	python::class_<CvEffectInfo, python::bases<CvInfoBase, CvScalableInfo> >("CvEffectInfo", python::no_init)

		.def("getPath", &CvEffectInfo::getPath, "string ()")
		.def("setPath", &CvEffectInfo::setPath, "void (string)")
	;


	python::class_<CvControlInfo, python::bases<CvInfoBase> >("CvControlInfo", python::no_init)

		.def("getActionInfoIndex", &CvControlInfo::getActionInfoIndex, "int ()")
	;
}
