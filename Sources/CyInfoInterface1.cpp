#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvInfos.h"

//
// Python interface for info classes (formerly structs)
// These are simple enough to be exposed directly - no wrappers
//

void CyInfoPythonInterface1()
{
	OutputDebugString("Python Extension Module - CyInfoPythonInterface1\n");

	python::class_<CvInfoBase>("CvInfoBase")

		.def("isGraphicalOnly", &CvInfoBase::isGraphicalOnly, "bool ()")

		.def("getType", &CvInfoBase::getType, "string ()")
		.def("getButton", &CvInfoBase::getButton, "string ()")

		.def("getTextKey", &CvInfoBase::pyGetTextKey, "wstring ()")
		.def("getText", &CvInfoBase::pyGetText, "wstring ()")
		.def("getDescription", &CvInfoBase::pyGetDescription, "wstring ()")
		.def("getDescriptionForm", &CvInfoBase::pyGetDescriptionForm, "wstring ()")
		.def("getCivilopedia", &CvInfoBase::pyGetCivilopedia, "wstring ()")
		.def("getStrategy", &CvInfoBase::pyGetStrategy, "wstring ()")
		.def("getHelp", &CvInfoBase::pyGetHelp, "wstring ()")
		;

	python::class_<CvScalableInfo>("CvScalableInfo")
		.def("getScale", &CvScalableInfo::getScale, "float ()")
		;

	python::class_<CvSpecialistInfo, python::bases<CvInfoBase> >("CvSpecialistInfo")

		.def("getGreatPeopleUnitType", &CvSpecialistInfo::getGreatPeopleUnitType, "int ()")
		.def("getGreatPeopleRateChange", &CvSpecialistInfo::getGreatPeopleRateChange, "int ()")
		.def("getMissionType", &CvSpecialistInfo::getMissionType, "int ()")

		.def("isVisible", &CvSpecialistInfo::isVisible, "bool ()")
		.def("isSlave", &CvSpecialistInfo::isSlave, "bool ()")

		.def("getYieldChange", &CvSpecialistInfo::getYieldChange, "int (int i)")
		.def("getCommerceChange", &CvSpecialistInfo::getCommerceChange, "int (int i)")
		.def("getExperience", &CvSpecialistInfo::getExperience, "int ()")
		.def("getFlavorValue", &CvSpecialistInfo::getFlavorValue, "int (int i)")

		.def("getTexture", &CvSpecialistInfo::getTexture, "string ()")
	;

	python::class_<CvTechInfo, python::bases<CvInfoBase> >("CvTechInfo")
		.def("getAdvisorType", &CvTechInfo::getAdvisorType, "int ()")
		.def("getResearchCost", &CvTechInfo::getResearchCost, "int ()")
		.def("getEra", &CvTechInfo::getEra, "int ()")
		.def("getTradeRoutes", &CvTechInfo::getTradeRoutes, "int ()")
		.def("getFeatureProductionModifier", &CvTechInfo::getFeatureProductionModifier, "int ()")
		.def("getWorkerSpeedModifier", &CvTechInfo::getWorkerSpeedModifier, "int ()")
		.def("getFirstFreeUnit", &CvTechInfo::getFirstFreeUnit, "int ()")
		.def("getHealth", &CvTechInfo::getHealth, "int ()")
		.def("getHappiness", &CvTechInfo::getHappiness, "int ()")
		.def("getFirstFreeTechs", &CvTechInfo::getFirstFreeTechs, "int ()")
		.def("getPowerValue", &CvTechInfo::getPowerValue, "int ()")

		.def("getGridX", &CvTechInfo::getGridX, "int ()")
		.def("getGridY", &CvTechInfo::getGridY, "int ()")

		.def("isGlobal", &CvTechInfo::isGlobal, "bool ()")
		.def("isRepeat", &CvTechInfo::isRepeat, "bool ()")
		.def("isTrade", &CvTechInfo::isTrade, "bool ()")
		.def("isDisable", &CvTechInfo::isDisable, "bool ()")
		.def("isGoodyTech", &CvTechInfo::isGoodyTech, "bool ()")
		.def("isExtraWaterSeeFrom", &CvTechInfo::isExtraWaterSeeFrom, "bool ()")
		.def("isMapCentering", &CvTechInfo::isMapCentering, "bool ()")
		.def("isMapVisible", &CvTechInfo::isMapVisible, "bool ()")
		.def("isMapTrading", &CvTechInfo::isMapTrading, "bool ()")
		.def("isTechTrading", &CvTechInfo::isTechTrading, "bool ()")
		.def("isGoldTrading", &CvTechInfo::isGoldTrading, "bool ()")
		.def("isOpenBordersTrading", &CvTechInfo::isOpenBordersTrading, "bool ()")
		.def("isDefensivePactTrading", &CvTechInfo::isDefensivePactTrading, "bool ()")
		.def("isPermanentAllianceTrading", &CvTechInfo::isPermanentAllianceTrading, "bool ()")
		.def("isVassalStateTrading", &CvTechInfo::isVassalStateTrading, "bool ()")
		.def("isBridgeBuilding", &CvTechInfo::isBridgeBuilding, "bool ()")
		.def("isIrrigation", &CvTechInfo::isIrrigation, "bool ()")
		.def("isIgnoreIrrigation", &CvTechInfo::isIgnoreIrrigation, "bool ()")
		.def("isWaterWork", &CvTechInfo::isWaterWork, "bool ()")
		.def("isRiverTrade", &CvTechInfo::isRiverTrade, "bool ()")

		.def("getQuote", &CvTechInfo::getQuote, "wstring ()")
		.def("getSound", &CvTechInfo::getSound, "string ()")

		.def("getDomainExtraMoves", &CvTechInfo::getDomainExtraMoves, "int (int i)")
		.def("getFlavorValue", &CvTechInfo::getFlavorValue, "int (int i)")
		.def("getPrereqOrTechs", &CvTechInfo::cyGetPrereqOrTechs)
		.def("getPrereqAndTechs", &CvTechInfo::cyGetPrereqAndTechs)

		.def("isCommerceFlexible", &CvTechInfo::isCommerceFlexible, "bool (int i)")
		.def("isTerrainTrade", &CvTechInfo::isTerrainTrade, "bool (int i)")

		.def("getNumPrereqBuildings", &CvTechInfo::getNumPrereqBuildings, "int ()")
		.def("getPrereqBuildingType", &CvTechInfo::getPrereqBuildingType, "int (int i)")
		.def("getPrereqBuildingMinimumRequired", &CvTechInfo::getPrereqBuildingMinimumRequired, "int (int i)")
		.def("getNumPrereqOrBuildings", &CvTechInfo::getNumPrereqOrBuildings, "int ()")
		.def("getPrereqOrBuildingType", &CvTechInfo::getPrereqOrBuildingType, "int (int i)")
		.def("getPrereqOrBuildingMinimumRequired", &CvTechInfo::getPrereqOrBuildingMinimumRequired, "int (int i)")
		;

	python::class_<CvPromotionInfo, python::bases<CvInfoBase> >("CvPromotionInfo")

		.def("getPrereqPromotion", &CvPromotionInfo::getPrereqPromotion, "int ()")
		.def("getPrereqOrPromotion1", &CvPromotionInfo::getPrereqOrPromotion1, "int ()")
		.def("getPrereqOrPromotion2", &CvPromotionInfo::getPrereqOrPromotion2, "int ()")
		.def("getActionInfoIndex", &CvPromotionInfo::getActionInfoIndex, "int ()")

		.def("getTechPrereq", &CvPromotionInfo::getTechPrereq, "int ()")
		.def("getStateReligionPrereq", &CvPromotionInfo::getStateReligionPrereq, "int ()")
		.def("getVisibilityChange", &CvPromotionInfo::getVisibilityChange, "int ()")
		.def("getMoveDiscountChange", &CvPromotionInfo::getMoveDiscountChange, "int ()")
		.def("getWithdrawalChange", &CvPromotionInfo::getWithdrawalChange, "int ()")
		.def("getCollateralDamageChange", &CvPromotionInfo::getCollateralDamageChange, "int ()")
		.def("getFirstStrikesChange", &CvPromotionInfo::getFirstStrikesChange, "int ()")
		.def("getChanceFirstStrikesChange", &CvPromotionInfo::getChanceFirstStrikesChange, "int ()")
		.def("getEnemyHealChange", &CvPromotionInfo::getEnemyHealChange, "int ()")
		.def("getNeutralHealChange", &CvPromotionInfo::getNeutralHealChange, "int ()")
		.def("getFriendlyHealChange", &CvPromotionInfo::getFriendlyHealChange, "int ()")
		.def("getCombatPercent", &CvPromotionInfo::getCombatPercent, "int ()")
		.def("getCityAttackPercent", &CvPromotionInfo::getCityAttackPercent, "int ()")
		.def("getCityDefensePercent", &CvPromotionInfo::getCityDefensePercent, "int ()")
		.def("getHillsDefensePercent", &CvPromotionInfo::getHillsDefensePercent, "int ()")
		.def("getCommandType", &CvPromotionInfo::getCommandType, "int ()")
		.def("getRevoltProtection", &CvPromotionInfo::getRevoltProtection, "int ()")
		.def("getPillageChange", &CvPromotionInfo::getPillageChange, "int ()")
		.def("getUpgradeDiscount", &CvPromotionInfo::getUpgradeDiscount, "int ()")

		.def("isLeader", &CvPromotionInfo::isLeader, "bool ()")
		.def("isRiver", &CvPromotionInfo::isRiver, "bool ()")

		.def("getObsoleteTech", &CvPromotionInfo::getObsoleteTech, "int ()")

		.def("getSound", &CvPromotionInfo::getSound, "string ()")

		// Arrays

		.def("getTerrainDefensePercent", &CvPromotionInfo::getTerrainDefensePercent, "int (int i)")
		.def("getFeatureDefensePercent", &CvPromotionInfo::getFeatureDefensePercent, "int (int i)")

		.def("getUnitCombat", &CvPromotionInfo::getUnitCombat, "bool (int i)")
		.def("getPromotionLine", &CvPromotionInfo::getPromotionLine, "int ()")

		.def("isQualifiedUnitCombatType", &CvPromotionInfo::isQualifiedUnitCombatType, "bool (int i)")
		.def("isStatus", &CvPromotionInfo::isStatus, "bool ()")

		.def("isEquipment", &CvPromotionInfo::isEquipment, "bool ()")
		.def("isAffliction", &CvPromotionInfo::isAffliction, "bool ()")
		.def("getAddsBuildType", &CvPromotionInfo::getAddsBuildType, "int (int i)")
		.def("getNumAddsBuildTypes", &CvPromotionInfo::getNumAddsBuildTypes, "int ()")
		;

	python::class_<CvMissionInfo, python::bases<CvInfoBase> >("CvMissionInfo")
		.def("getTime", &CvMissionInfo::getTime, "int ()")

		.def("isSound", &CvMissionInfo::isSound, "bool ()")
		.def("isTarget", &CvMissionInfo::isTarget, "bool ()")
		.def("isBuild", &CvMissionInfo::isBuild, "bool ()")
		.def("getVisible", &CvMissionInfo::getVisible, "bool ()")
		;

	python::class_<CvActionInfo>("CvActionInfo")
		.def("getMissionData", &CvActionInfo::getMissionData, "int ()")
		.def("getAutomateType", &CvActionInfo::getAutomateType, "int ()")

		.def("getMissionType", &CvActionInfo::getMissionType, "int ()")
		.def("getCommandType", &CvActionInfo::getCommandType, "int ()")

		.def("isVisible", &CvActionInfo::isVisible, "bool ()")

		.def("getHotKey", &CvActionInfo::getHotKey, "string ()")
		.def("getButton", &CvActionInfo::getButton, "string ()")
		;

	python::class_<CvUnitInfo, python::bases<CvInfoBase, CvScalableInfo> >("CvUnitInfo")

		.def("getMaxGlobalInstances", &CvUnitInfo::getMaxGlobalInstances, "int ()")
		.def("getMaxPlayerInstances", &CvUnitInfo::getMaxPlayerInstances, "int ()")
		.def("getProductionCost", &CvUnitInfo::getProductionCost, "int ()")
		.def("getMinAreaSize", &CvUnitInfo::getMinAreaSize, "int ()")
		.def("getMoves", &CvUnitInfo::getMoves, "int ()")
		.def("getAirRange", &CvUnitInfo::getAirRange, "int ()")
		.def("getWorkRate", &CvUnitInfo::getWorkRate, "int ()")
		.def("getGreatWorkCulture", &CvUnitInfo::getGreatWorkCulture, "int ()")
		.def("getEspionagePoints", &CvUnitInfo::getEspionagePoints, "int ()")
		.def("getCombat", &CvUnitInfo::getCombat, "int ()")
		.def("setCombat", &CvUnitInfo::setCombat, "void (int)")
		.def("getAirCombat", &CvUnitInfo::getAirCombat, "int ()")
		.def("getFirstStrikes", &CvUnitInfo::getFirstStrikes, "int ()")
		.def("getChanceFirstStrikes", &CvUnitInfo::getChanceFirstStrikes, "int ()")
		.def("getCollateralDamage", &CvUnitInfo::getCollateralDamage, "int ()")

		.def("getCargoSpace", &CvUnitInfo::getCargoSpace, "int ()")
		.def("getConscriptionValue", &CvUnitInfo::getConscriptionValue, "int ()")
		.def("getPowerValue", &CvUnitInfo::getPowerValue, "int ()")
		.def("getSpecialUnitType", &CvUnitInfo::getSpecialUnitType, "int ()")
		.def("getUnitCaptureType", &CvUnitInfo::getUnitCaptureType, "int ()")
		.def("getUnitCombatType", &CvUnitInfo::getUnitCombatType, "int ()")
		.def("getDomainType", &CvUnitInfo::getDomainType, "int ()")
		.def("getDefaultUnitAIType", &CvUnitInfo::getDefaultUnitAIType, "int ()")
		.def("getAdvisorType", &CvUnitInfo::getAdvisorType, "int ()")

		.def("getForceObsoleteTech", &CvUnitInfo::getForceObsoleteTech, "int ()")
		.def("isStateReligion", &CvUnitInfo::isStateReligion, "bool ()")

		.def("getHolyCity", &CvUnitInfo::getHolyCity, "int ()")
		.def("getReligionType", &CvUnitInfo::getReligionType, "int ()")
		.def("getStateReligion", &CvUnitInfo::getStateReligion, "int ()")
		.def("getPrereqReligion", &CvUnitInfo::getPrereqReligion, "int ()")
		.def("getPrereqCorporation", &CvUnitInfo::getPrereqCorporation, "int ()")
		.def("getPrereqOrBuildingsNum", &CvUnitInfo::getPrereqOrBuildingsNum, "int ()")
		.def("getPrereqOrBuilding", &CvUnitInfo::getPrereqOrBuilding, "int (int)")
		.def("getPrereqAndTech", &CvUnitInfo::getPrereqAndTech, "int ()")
		.def("getPrereqAndBonus", &CvUnitInfo::getPrereqAndBonus, "int ()")
		.def("getNumUnitNames", &CvUnitInfo::getNumUnitNames, "int ()")
		.def("getCommandType", &CvUnitInfo::getCommandType, "int ()")

		.def("isFoodProduction", &CvUnitInfo::isFoodProduction, "bool ()")
		.def("isOnlyDefensive", &CvUnitInfo::isOnlyDefensive, "bool ()")
		.def("isMilitaryProduction", &CvUnitInfo::isMilitaryProduction, "bool ()")
		.def("isSpy", &CvUnitInfo::isSpy, "bool ()")
		.def("isInvestigate", &CvUnitInfo::isInvestigate, "bool ()")
		.def("isFound", &CvUnitInfo::isFound, "bool ()")
		.def("isGoldenAge", &CvUnitInfo::isGoldenAge, "bool ()")
		.def("isInvisible", &CvUnitInfo::isInvisible, "bool ()")
		.def("isIgnoreBuildingDefense", &CvUnitInfo::isIgnoreBuildingDefense, "bool ()")
		.def("isNukeImmune", &CvUnitInfo::isNukeImmune, "bool ()")
		.def("isHiddenNationality", &CvUnitInfo::isHiddenNationality, "bool ()")

		// Arrays
		.def("getPrereqAndTechs", &CvUnitInfo::cyGetPrereqAndTechs)
		.def("getPrereqOrBonuses", &CvUnitInfo::cyGetPrereqOrBonuses)
		.def("getFlavorValue", &CvUnitInfo::getFlavorValue, "int (int i)")
		.def("getBonusProductionModifier", &CvUnitInfo::getBonusProductionModifier, "int (int i)")

		.def("isPrereqOrCivics", &CvUnitInfo::isPrereqOrCivics, "bool (int i)")

		.def("getPrereqAndBuilding", &CvUnitInfo::getPrereqAndBuilding, "int (int i)")
		.def("getNumPrereqAndBuildings", &CvUnitInfo::getNumPrereqAndBuildings, "int ()")

		.def("getUnitUpgrade", &CvUnitInfo::getUnitUpgrade, "int (int i)")
		.def("getNumUnitUpgrades", &CvUnitInfo::getNumUnitUpgrades, "int ()")
		.def("getUnitAIType", &CvUnitInfo::getUnitAIType, "bool (int i)")
		.def("getBuild", &CvUnitInfo::getBuild, "int (int i)")
		.def("getNumBuilds", &CvUnitInfo::getNumBuilds, "short ()")
		.def("hasBuild", &CvUnitInfo::hasBuild, "bool (BuildTypes eBuild)")
		.def("getReligionSpreads", &CvUnitInfo::getReligionSpreads, "int (int i)")
		.def("getCorporationSpreads", &CvUnitInfo::getCorporationSpreads, "int (int i)")
		.def("getGreatPeoples", &CvUnitInfo::getGreatPeoples, "bool (int i)")
		.def("getBuildings", &CvUnitInfo::getBuildings, "int (int i)")
		.def("getHasBuilding", &CvUnitInfo::getHasBuilding, "bool (int i)")
		.def("getNumBuildings", &CvUnitInfo::getNumBuildings, "int ()")
		.def("getFreePromotions", &CvUnitInfo::getFreePromotions, "bool (int i)")
		//.def("getTerrainImpassable", &CvUnitInfo::getTerrainImpassable, "bool (int i)")
		//.def("getFeatureImpassable", &CvUnitInfo::getFeatureImpassable, "bool (int i)")
		.def("getUnitNames", &CvUnitInfo::getUnitNames, "string (int i)")
		//TB SubCombat Mod begin  TB Combat Mods Begin
		//boolean vectors
		.def("isSubCombatType", &CvUnitInfo::isSubCombatType, "int (int i)")
		.def("isQualifiedPromotionType", &CvUnitInfo::isQualifiedPromotionType, "bool (int i)")
		.def("hasUnitCombat", &CvUnitInfo::hasUnitCombat, "bool (UnitCombatTypes eUnitCombat)")
		//TB Combat Mods End  TB SubCombat Mod end
		
		.def("getTrainCondition", &CvUnitInfo::getTrainCondition, python::return_value_policy<python::reference_existing_object>())		
		;

	python::class_<CvSpecialUnitInfo, python::bases<CvInfoBase> >("CvSpecialUnitInfo")
		.def("isValid", &CvSpecialUnitInfo::isValid, "bool ()")
		;

	python::class_<CvCivicOptionInfo, python::bases<CvInfoBase> >("CvCivicOptionInfo")
		//.def("getTraitNoUpkeep", &CvCivicOptionInfo::getTraitNoUpkeep, "bool (int i)")
		;

	python::class_<CvCivicInfo, python::bases<CvInfoBase> >("CvCivicInfo")

		.def("getCivicOptionType", &CvCivicInfo::getCivicOptionType, "int ()")
		.def("getUpkeep", &CvCivicInfo::getUpkeep, "int ()")
		.def("getGreatPeopleRateModifier", &CvCivicInfo::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CvCivicInfo::getGreatGeneralRateModifier, "int ()")
		.def("getExtraHealth", &CvCivicInfo::getExtraHealth, "int ()")
		.def("getFreeExperience", &CvCivicInfo::getFreeExperience, "int ()")
		.def("getWorkerSpeedModifier", &CvCivicInfo::getWorkerSpeedModifier, "int ()")
		.def("getMilitaryProductionModifier", &CvCivicInfo::getMilitaryProductionModifier, "int ()")
		.def("getWarWearinessModifier", &CvCivicInfo::getWarWearinessModifier, "int ()")
		.def("getFreeSpecialist", &CvCivicInfo::getFreeSpecialist, "int ()")
		.def("getTradeRoutes", &CvCivicInfo::getTradeRoutes, "int ()")
		.def("getTechPrereq", &CvCivicInfo::getTechPrereq, "int ()")
		.def("getStateReligionHappiness", &CvCivicInfo::getStateReligionHappiness, "int ()")
		.def("getStateReligionUnitProductionModifier", &CvCivicInfo::getStateReligionUnitProductionModifier, "int ()")
		.def("getStateReligionBuildingProductionModifier", &CvCivicInfo::getStateReligionBuildingProductionModifier, "int ()")
		.def("getStateReligionFreeExperience", &CvCivicInfo::getStateReligionFreeExperience, "int ()")

		.def("getRevIdxLocal", &CvCivicInfo::getRevIdxLocal, "int ()")
		.def("getRevIdxNational", &CvCivicInfo::getRevIdxNational, "int ()")
		.def("getRevIdxDistanceModifier", &CvCivicInfo::getRevIdxDistanceModifier, "int ()")
		.def("getRevIdxHolyCityGood", &CvCivicInfo::getRevIdxHolyCityGood, "int ()")
		.def("getRevIdxHolyCityBad", &CvCivicInfo::getRevIdxHolyCityBad, "int ()")
		.def("getRevIdxSwitchTo", &CvCivicInfo::getRevIdxSwitchTo, "int ()")
		.def("getRevIdxNationalityMod", &CvCivicInfo::getRevIdxNationalityMod, "float ()")
		.def("getRevIdxBadReligionMod", &CvCivicInfo::getRevIdxBadReligionMod, "float ()")
		.def("getRevIdxGoodReligionMod", &CvCivicInfo::getRevIdxGoodReligionMod, "float ()")
		.def("getRevViolentMod", &CvCivicInfo::getRevViolentMod, "float ()")
		.def("getRevReligiousFreedom", &CvCivicInfo::getRevReligiousFreedom, "int ()")
		.def("getRevLaborFreedom", &CvCivicInfo::getRevLaborFreedom, "int ()")
		.def("getRevEnvironmentalProtection", &CvCivicInfo::getRevEnvironmentalProtection, "int ()")
		.def("getRevDemocracyLevel", &CvCivicInfo::getRevDemocracyLevel, "int ()")

		.def("isCommunism", &CvCivicInfo::isCommunism, "bool ()")
		.def("isFreeSpeech", &CvCivicInfo::isFreeSpeech, "bool ()")
		.def("isCanDoElection", &CvCivicInfo::isCanDoElection, "bool ()")

		.def("isBuildingOnlyHealthy", &CvCivicInfo::isBuildingOnlyHealthy, "bool ()")
		.def("isStateReligion", &CvCivicInfo::isStateReligion, "bool ()")

		// Arrays

		.def("getYieldModifier", &CvCivicInfo::getYieldModifier, "int (int i)")
		.def("getCommerceModifier", &CvCivicInfo::getCommerceModifier, "int (int i)")
		.def("getSpecialistExtraCommerce", &CvCivicInfo::getSpecialistExtraCommerce, "int (int i)")

		.def("isSpecialistValid", &CvCivicInfo::isSpecialistValid, "bool (int i)")

		.def("getImprovementYieldChanges", &CvCivicInfo::getImprovementYieldChanges, "int (int i, int j)")
		;

	python::class_<CvBuildingInfo, python::bases<CvInfoBase, CvScalableInfo> >("CvBuildingInfo")

		.def("getMaxGlobalInstances", &CvBuildingInfo::getMaxGlobalInstances, "int ()")
		.def("getMaxTeamInstances", &CvBuildingInfo::getMaxTeamInstances, "int ()")
		.def("getMaxPlayerInstances", &CvBuildingInfo::getMaxPlayerInstances, "int ()")
		.def("isNoLimit", &CvBuildingInfo::isNoLimit, "bool ()")
		.def("getVictoryThreshold", &CvBuildingInfo::getVictoryThreshold, "int (int i)")

		.def("getVictoryPrereq", &CvBuildingInfo::getVictoryPrereq, "int ()")
		.def("getObsoleteTech", &CvBuildingInfo::getObsoleteTech, "int ()")
		.def("getPrereqAndTech", &CvBuildingInfo::getPrereqAndTech, "int ()")
		.def("getFreeBonus", &CvBuildingInfo::getFreeBonus, "int ()")
		.def("getNumExtraFreeBonuses", &CvBuildingInfo::getNumExtraFreeBonuses, "int ()")
		.def("getExtraFreeBonus", &CvBuildingInfo::getExtraFreeBonus, "int (int i)")
		.def("getFreeBuilding", &CvBuildingInfo::getFreeBuilding, "int ()")
		.def("getFreePromotion", &CvBuildingInfo::getFreePromotion, "int ()")
		.def("getCivic", &CvBuildingInfo::getCivicOption, "int ()")
		.def("getProductionCost", &CvBuildingInfo::getProductionCost, "int ()")
		.def("getMinAreaSize", &CvBuildingInfo::getMinAreaSize, "int ()")
		.def("getMinLatitude", &CvBuildingInfo::getMinLatitude, "int ()")
		.def("getMaxLatitude", &CvBuildingInfo::getMaxLatitude, "int ()")
		.def("getGreatPeopleRateModifier", &CvBuildingInfo::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CvBuildingInfo::getGreatGeneralRateModifier, "int ()")
		.def("getAnarchyModifier", &CvBuildingInfo::getAnarchyModifier, "int ()")
		.def("getGoldenAgeModifier", &CvBuildingInfo::getGoldenAgeModifier, "int ()")
		.def("getFreeExperience", &CvBuildingInfo::getFreeExperience, "int ()")
		.def("getGlobalFreeExperience", &CvBuildingInfo::getGlobalFreeExperience, "int ()")
		.def("getFoodKept", &CvBuildingInfo::getFoodKept, "int ()")
		.def("getAirModifier", &CvBuildingInfo::getAirModifier, "int ()")
		.def("getNukeModifier", &CvBuildingInfo::getNukeModifier, "int ()")
		.def("getFreeSpecialist", &CvBuildingInfo::getFreeSpecialist, "int ()")
		.def("getHappiness", &CvBuildingInfo::getHappiness, "int ()")
		.def("getStateReligionHappiness", &CvBuildingInfo::getStateReligionHappiness, "int ()")
		.def("getWorkerSpeedModifier", &CvBuildingInfo::getWorkerSpeedModifier, "int ()")
		.def("getMilitaryProductionModifier", &CvBuildingInfo::getMilitaryProductionModifier, "int ()")
		.def("getSpaceProductionModifier", &CvBuildingInfo::getSpaceProductionModifier, "int ()")
		.def("getTradeRoutes", &CvBuildingInfo::getTradeRoutes, "int ()")
		.def("getCoastalTradeRoutes", &CvBuildingInfo::getCoastalTradeRoutes, "int ()")
		.def("getGlobalTradeRoutes", &CvBuildingInfo::getGlobalTradeRoutes, "int ()")
		.def("getTradeRouteModifier", &CvBuildingInfo::getTradeRouteModifier, "int ()")
		.def("getForeignTradeRouteModifier", &CvBuildingInfo::getForeignTradeRouteModifier, "int ()")
		.def("getPowerValue", &CvBuildingInfo::getPowerValue, "int ()")
		.def("getSpecialBuildingType", &CvBuildingInfo::getSpecialBuilding)
		.def("getAdvisorType", &CvBuildingInfo::getAdvisorType, "int ()")
		.def("getHolyCity", &CvBuildingInfo::getHolyCity, "int ()")
		.def("getReligionType", &CvBuildingInfo::getReligionType, "int ()")
		.def("getPrereqStateReligion", &CvBuildingInfo::getPrereqStateReligion, "int ()")
		.def("getPrereqReligion", &CvBuildingInfo::getPrereqReligion, "int ()")
		.def("getPrereqCorporation", &CvBuildingInfo::getPrereqCorporation, "int ()")
		.def("getFoundsCorporation", &CvBuildingInfo::getFoundsCorporation, "int ()")
		.def("getGlobalReligionCommerce", &CvBuildingInfo::getGlobalReligionCommerce, "int ()")
		.def("getPrereqAndBonus", &CvBuildingInfo::getPrereqAndBonus, "int ()")
		.def("getGreatPeopleUnitType", &CvBuildingInfo::getGreatPeopleUnitType, "int ()")
		.def("getGreatPeopleRateChange", &CvBuildingInfo::getGreatPeopleRateChange, "int ()")
		.def("getMaintenanceModifier", &CvBuildingInfo::getMaintenanceModifier, "int ()")
		.def("getWarWearinessModifier", &CvBuildingInfo::getWarWearinessModifier, "int ()")
		.def("getGlobalWarWearinessModifier", &CvBuildingInfo::getGlobalWarWearinessModifier, "int ()")
		.def("getEnemyWarWearinessModifier", &CvBuildingInfo::getEnemyWarWearinessModifier, "int ()")
		.def("getHealth", &CvBuildingInfo::getHealth, "int ()")
		.def("getGlobalHealth", &CvBuildingInfo::getGlobalHealth, "int ()")
		.def("getDefenseModifier", &CvBuildingInfo::getDefenseModifier, "int ()")
		.def("getAllCityDefenseModifier", &CvBuildingInfo::getAllCityDefenseModifier, "int ()")
		.def("getEspionageDefenseModifier", &CvBuildingInfo::getEspionageDefenseModifier, "int ()")
		.def("getRevIdxLocal", &CvBuildingInfo::getRevIdxLocal, "int ()")
		.def("getRevIdxNational", &CvBuildingInfo::getRevIdxNational, "int ()")
		.def("getRevIdxDistanceModifier", &CvBuildingInfo::getRevIdxDistanceModifier, "int ()")
		.def("getMissionType", &CvBuildingInfo::getMissionType, "int ()")
		.def("getVoteSourceType", &CvBuildingInfo::getVoteSourceType, "int ()")
		.def("getPrereqVicinityBonus", &CvBuildingInfo::getPrereqVicinityBonus, "int ()")
		.def("getPrereqRawVicinityBonus", &CvBuildingInfo::getPrereqRawVicinityBonus, "int ()")
		.def("getHurryCostModifier", &CvBuildingInfo::getHurryCostModifier, "int ()")
		.def("getHurryAngerModifier", &CvBuildingInfo::getHurryAngerModifier, "int ()")

		.def("isAutoBuild", &CvBuildingInfo::isAutoBuild, "bool ()")
		.def("isWater", &CvBuildingInfo::isWater, "bool ()")
		.def("isRiver", &CvBuildingInfo::isRiver, "bool ()")
		.def("isFreshWater", &CvBuildingInfo::isFreshWater, "bool ()")
		.def("isPower", &CvBuildingInfo::isPower, "bool ()")
		.def("isDirtyPower", &CvBuildingInfo::isDirtyPower, "bool ()")
		.def("isAreaCleanPower", &CvBuildingInfo::isAreaCleanPower, "bool ()")
		.def("isForceTeamVoteEligible", &CvBuildingInfo::isForceTeamVoteEligible, "bool ()")
		.def("isCapital", &CvBuildingInfo::isCapital, "bool ()")
		.def("isGovernmentCenter", &CvBuildingInfo::isGovernmentCenter, "bool ()")
		.def("isGoldenAge", &CvBuildingInfo::isGoldenAge, "bool ()")
		.def("isMapCentering", &CvBuildingInfo::isMapCentering, "bool ()")
		.def("isNoUnhappiness", &CvBuildingInfo::isNoUnhappiness, "bool ()")
		.def("isBuildingOnlyHealthy", &CvBuildingInfo::isBuildingOnlyHealthy, "bool ()")
		.def("isNukeImmune", &CvBuildingInfo::isNukeImmune, "bool ()")
		.def("isAllowsNukes", &CvBuildingInfo::isAllowsNukes, "bool ()")
		.def("isAnyTechYieldChanges", &CvBuildingInfo::isAnyTechYieldChanges, "bool ()")
		.def("isAnyTechYieldModifiers", &CvBuildingInfo::isAnyTechYieldModifiers, "bool ()")
		.def("isAnyTechCommerceChanges", &CvBuildingInfo::isAnyTechCommerceChanges, "bool ()")
		.def("isAnyTechCommerceModifiers", &CvBuildingInfo::isAnyTechCommerceModifiers, "bool ()")
		.def("isAnyTechSpecialistChanges", &CvBuildingInfo::isAnyTechSpecialistChanges, "bool ()")


		.def("getHotKey", &CvBuildingInfo::getHotKey, "string ()")
		.def("getArtDefineTag", &CvBuildingInfo::getArtDefineTag, "string ()")
		.def("getMovie", &CvBuildingInfo::getMovie, "string ()")

		// Arrays

		.def("getYieldChange", &CvBuildingInfo::getYieldChange, "int (int i)")
		.def("getYieldPerPopChange", &CvBuildingInfo::getYieldPerPopChange, "int (int i)")
		.def("getYieldModifier", &CvBuildingInfo::getYieldModifier, "int (int i)")
		.def("getSeaPlotYieldChange", &CvBuildingInfo::getSeaPlotYieldChange, "int (int i)")
		.def("getCommerceChange", &CvBuildingInfo::getCommerceChange, "int (int i)")
		.def("getCommercePerPopChange", &CvBuildingInfo::getCommercePerPopChange, "int (int i)")
		.def("getCommerceModifier", &CvBuildingInfo::getCommerceModifier, "int (int i)")
		.def("getGlobalCommerceModifier", &CvBuildingInfo::getGlobalCommerceModifier, "int (int i)")
		.def("getCommerceHappiness", &CvBuildingInfo::getCommerceHappiness, "int (int i)")
		.def("getCommerceChangeDoubleTime", &CvBuildingInfo::getCommerceChangeDoubleTime, "int (int i)")
		.def("getSpecialistCount", &CvBuildingInfo::getSpecialistCount, "int (int i)")
		.def("getFreeSpecialistCount", &CvBuildingInfo::getFreeSpecialistCount, "int (int i)")
		.def("getBonusHealthChanges", &CvBuildingInfo::getBonusHealthChanges, "int (int i)")
		.def("getBonusHappinessChanges", &CvBuildingInfo::getBonusHappinessChanges, "int (int i)")
		.def("getBonusProductionModifier", &CvBuildingInfo::getBonusProductionModifier, "int (int i)")
		.def("getDomainFreeExperience", &CvBuildingInfo::getDomainFreeExperience, "int (int i)")
		.def("getDomainProductionModifier", &CvBuildingInfo::getDomainProductionModifier, "int (int i)")
		.def("getPrereqAndTechs", &CvBuildingInfo::cyGetPrereqAndTechs)
		.def("getPrereqOrBonuses", &CvBuildingInfo::cyGetPrereqOrBonuses)
		.def("getPrereqOrVicinityBonuses", &CvBuildingInfo::cyGetPrereqOrVicinityBonuses)
		.def("getPrereqOrRawVicinityBonuses", &CvBuildingInfo::cyGetPrereqOrRawVicinityBonuses)
		.def("getTechHappinessChanges", &CvBuildingInfo::cyGetTechHappinessChanges)
		.def("getTechHealthChanges", &CvBuildingInfo::cyGetTechHealthChanges)
		.def("getTechYieldChange", &CvBuildingInfo::getTechYieldChange, "int (int i, int j)")
		.def("getTechYieldModifier", &CvBuildingInfo::getTechYieldModifier, "int (int i, int j)")
		.def("getTechCommerceChange", &CvBuildingInfo::getTechCommerceChange, "int (int i, int j)")
		.def("getTechCommerceModifier", &CvBuildingInfo::getTechCommerceModifier, "int (int i, int j)")
		.def("getTechSpecialistChange", &CvBuildingInfo::getTechSpecialistChange, "int (int i, int j)")

		.def("getNumPrereqOrBonuses", &CvBuildingInfo::getNumPrereqOrBonuses, "int ()")
		.def("getPrereqNumOfBuilding", &CvBuildingInfo::getPrereqNumOfBuilding, "int (int i)")
		.def("getPrereqNumOfBuildings", &CvBuildingInfo::cyGetPrereqNumOfBuildings)
		.def("getFlavorValue", &CvBuildingInfo::getFlavorValue, "int (int i)")
		.def("getImprovementFreeSpecialist", &CvBuildingInfo::getImprovementFreeSpecialist, "int (int i)")

		.def("isPrereqOrCivics", &CvBuildingInfo::isPrereqOrCivics, "bool (int i)")
		.def("isPrereqAndCivics", &CvBuildingInfo::isPrereqAndCivics, "bool (int i)")

		.def("isPrereqOrTerrain", &CvBuildingInfo::isPrereqOrTerrain, "bool (int i)")
		.def("isPrereqAndTerrain", &CvBuildingInfo::isPrereqAndTerrain, "bool (int i)")

		.def("isPrereqOrImprovement", &CvBuildingInfo::isPrereqOrImprovement, "bool (int i)")
		.def("isPrereqOrFeature", &CvBuildingInfo::isPrereqOrFeature, "bool (int i)")

		.def("getPrereqOrBuilding", &CvBuildingInfo::getPrereqOrBuilding, "int (int i)")
		.def("getNumPrereqOrBuilding", &CvBuildingInfo::getNumPrereqOrBuilding, "short ()")
		.def("isPrereqOrBuilding", &CvBuildingInfo::isPrereqOrBuilding, "bool (int i)")

		.def("getReplacementBuilding", &CvBuildingInfo::getReplacementBuilding, "int (int i)")
		.def("getNumReplacementBuilding", &CvBuildingInfo::getNumReplacementBuilding, "short ()")
		.def("getReplacedBuilding", &CvBuildingInfo::getReplacedBuilding, "int (int i)")
		.def("getNumReplacedBuilding", &CvBuildingInfo::getNumReplacedBuilding, "short ()")

		.def("getNumPopulationEmployed", &CvBuildingInfo::getNumPopulationEmployed, "int")

		.def("isCommerceFlexible", &CvBuildingInfo::isCommerceFlexible, "bool (int i)")
		.def("isCommerceChangeOriginalOwner", &CvBuildingInfo::isCommerceChangeOriginalOwner, "bool (int i)")

		.def("getPrereqInCityBuilding", &CvBuildingInfo::getPrereqInCityBuilding, "int (int i)")
		.def("getNumPrereqInCityBuildings", &CvBuildingInfo::getNumPrereqInCityBuildings, "short ()")
		.def("isPrereqInCityBuilding", &CvBuildingInfo::isPrereqInCityBuilding, "bool (int i)")

		.def("getBonusYieldModifier", &CvBuildingInfo::getBonusYieldModifier, "int (int i, int j)")

		.def("getProperties", &CvBuildingInfo::getProperties, python::return_value_policy<python::reference_existing_object>(), "CvProperties ()")

		//Extract map category of building
		.def("isMapType", &CvBuildingInfo::isMapType, "bool (int i)")

		.def("getConstructCondition", &CvBuildingInfo::getConstructCondition, python::return_value_policy<python::reference_existing_object>())

		//TB Combat Mods (Buildings) begin
		//integers
		//boolean vectors without delayed resolution
		//.def("getFreePromoType", &CvBuildingInfo::getFreePromoType, "int (int i)")
		//integer vectors with pairing method without delayed resolution
		//TB Combat Mods (Buildings) end
		;

	python::class_<CvSpecialBuildingInfo, python::bases<CvInfoBase> >("CvSpecialBuildingInfo")
		.def("getObsoleteTech", &CvSpecialBuildingInfo::getObsoleteTech, "int ()")
		.def("getTechPrereq", &CvSpecialBuildingInfo::getTechPrereq, "int ()")
		.def("isValid", &CvSpecialBuildingInfo::isValid, "bool ()")
		;

	python::class_<CvPromotionLineInfo, python::bases<CvInfoBase> >("CvPromotionLineInfo")
		.def("getPrereqTech", &CvPromotionLineInfo::getPrereqTech, "int ()")
		.def("getObsoleteTech", &CvPromotionLineInfo::getObsoleteTech, "int ()")
		.def("isBuildUp", &CvPromotionLineInfo::isBuildUp, "bool ()")
		;

	python::class_<CvUnitCombatInfo, python::bases<CvInfoBase> >("CvUnitCombatInfo")
		.def("getVisibilityChange", &CvUnitCombatInfo::getVisibilityChange, "int ()")
		.def("getMoveDiscountChange", &CvUnitCombatInfo::getMoveDiscountChange, "int ()")
		.def("getWithdrawalChange", &CvUnitCombatInfo::getWithdrawalChange, "int ()")
		.def("getCollateralDamageChange", &CvUnitCombatInfo::getCollateralDamageChange, "int ()")
		.def("getFirstStrikesChange", &CvUnitCombatInfo::getFirstStrikesChange, "int ()")
		.def("getChanceFirstStrikesChange", &CvUnitCombatInfo::getChanceFirstStrikesChange, "int ()")
		.def("getEnemyHealChange", &CvUnitCombatInfo::getEnemyHealChange, "int ()")
		.def("getNeutralHealChange", &CvUnitCombatInfo::getNeutralHealChange, "int ()")
		.def("getFriendlyHealChange", &CvUnitCombatInfo::getFriendlyHealChange, "int ()")
		.def("getCombatPercent", &CvUnitCombatInfo::getCombatPercent, "int ()")
		.def("getCityAttackPercent", &CvUnitCombatInfo::getCityAttackPercent, "int ()")
		.def("getCityDefensePercent", &CvUnitCombatInfo::getCityDefensePercent, "int ()")
		.def("getHillsDefensePercent", &CvUnitCombatInfo::getHillsDefensePercent, "int ()")
		.def("getRevoltProtection", &CvUnitCombatInfo::getRevoltProtection, "int ()")
		.def("getPillageChange", &CvUnitCombatInfo::getPillageChange, "int ()")
		.def("getUpgradeDiscount", &CvUnitCombatInfo::getUpgradeDiscount, "int ()")
		.def("getStrengthModifier", &CvUnitCombatInfo::getStrengthModifier, "int ()")
		.def("isRiver", &CvUnitCombatInfo::isRiver, "bool ()")
		;
}
