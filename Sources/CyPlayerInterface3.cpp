#include "CvGameCoreDLL.h"
#include "CyCity.h"
#include "CyPlayer.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// published python interface for CyPlayer
//

void CyPlayerPythonInterface3(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface1\n");

	// set the docstring of the current module scope
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("getAdvancedStartPoints", &CyPlayer::getAdvancedStartPoints, "int ()")
		.def("getAdvancedStartUnitCost", &CyPlayer::getAdvancedStartUnitCost, "int (int (UnitTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartCityCost", &CyPlayer::getAdvancedStartCityCost, "int (int (bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartPopCost", &CyPlayer::getAdvancedStartPopCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartCultureCost", &CyPlayer::getAdvancedStartCultureCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartBuildingCost", &CyPlayer::getAdvancedStartBuildingCost, "int (int (BuildingTypes) eUnit, bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartImprovementCost", &CyPlayer::getAdvancedStartImprovementCost, "int (int (ImprovementTypes) eImprovement, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartRouteCost", &CyPlayer::getAdvancedStartRouteCost, "int (int (RouteTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartTechCost", &CyPlayer::getAdvancedStartTechCost, "int (int (TechTypes) eUnit, bool bAdd)")
		.def("getAdvancedStartVisibilityCost", &CyPlayer::getAdvancedStartVisibilityCost, "int (CyPlot* pPlot)")

		.def("getEspionageSpending", &CyPlayer::getEspionageSpending, "int (PlayerTypes eIndex)")
		.def("canDoEspionageMission", &CyPlayer::canDoEspionageMission, "bool (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData)")
		.def("getEspionageMissionCost", &CyPlayer::getEspionageMissionCost, "int (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData)")
		.def("getEspionageSpendingWeightAgainstTeam", &CyPlayer::getEspionageSpendingWeightAgainstTeam, "int (TeamTypes eIndex)")
		.def("setEspionageSpendingWeightAgainstTeam", &CyPlayer::setEspionageSpendingWeightAgainstTeam, "void (TeamTypes eIndex, int iValue)")
		.def("changeEspionageSpendingWeightAgainstTeam", &CyPlayer::changeEspionageSpendingWeightAgainstTeam, "void (TeamTypes eIndex, int iChange)")

		.def("getGoldenAgeTurns", &CyPlayer::getGoldenAgeTurns, "int ()")
		.def("getGoldenAgeLength", &CyPlayer::getGoldenAgeLength, "int ()")
		.def("isGoldenAge", &CyPlayer::isGoldenAge, "bool ()")
		.def("changeGoldenAgeTurns", &CyPlayer::changeGoldenAgeTurns, "void (int iChange)")
		.def("changeNumUnitGoldenAges", &CyPlayer::changeNumUnitGoldenAges, "void (int iChange)")
		.def("getAnarchyTurns", &CyPlayer::getAnarchyTurns, "int ()")
		.def("isAnarchy", &CyPlayer::isAnarchy, "bool ()")
		.def("changeAnarchyTurns", &CyPlayer::changeAnarchyTurns, "void ()")
		.def("getAnarchyModifier", &CyPlayer::getAnarchyModifier, "int ()")
		.def("getGoldenAgeModifier", &CyPlayer::getGoldenAgeModifier, "int ()")
		.def("getGreatPeopleRateModifier", &CyPlayer::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CyPlayer::getGreatGeneralRateModifier, "int ()")
		.def("greatPeopleThresholdMilitary", &CyPlayer::greatPeopleThresholdMilitary, "int ()")
		.def("greatPeopleThresholdNonMilitary", &CyPlayer::greatPeopleThresholdNonMilitary, "int ()")
		.def("getGreatGeneralsThresholdModifier", &CyPlayer::getGreatGeneralsThresholdModifier, "int ()")
		.def("changeGreatGeneralsThresholdModifier", &CyPlayer::changeGreatGeneralsThresholdModifier, "void (int iChange)")

		.def("getFreeExperience", &CyPlayer::getFreeExperience, "int ()")
		.def("getFeatureProductionModifier", &CyPlayer::getFeatureProductionModifier, "int ()")
		.def("getWorkerSpeedModifier", &CyPlayer::getWorkerSpeedModifier, "int ()")
		.def("getMilitaryProductionModifier", &CyPlayer::getMilitaryProductionModifier, "int ()")
		.def("getSpaceProductionModifier", &CyPlayer::getSpaceProductionModifier, "int ()")

		.def("getRevIdxNational", &CyPlayer::getRevIdxNational, "int ()")
		.def("canFoundReligion", &CyPlayer::canFoundReligion, "bool ()")

		.def("getNumMilitaryUnits", &CyPlayer::getNumMilitaryUnits, "int ()")

		.def("getOverflowResearch", &CyPlayer::getOverflowResearch, "int ()")
		.def("isBuildingOnlyHealthy", &CyPlayer::isBuildingOnlyHealthy, "bool ()")

		.def("getTreasuryUpkeep", &CyPlayer::getTreasuryUpkeep, "int64 ()")
		.def("getTotalMaintenance", &CyPlayer::getTotalMaintenance, "int ()")
		.def("getLevelExperienceModifier", &CyPlayer::getLevelExperienceModifier, "int ()")

		.def("getExtraHealth", &CyPlayer::getExtraHealth, "int ()")
		.def("changeExtraHealth", &CyPlayer::changeExtraHealth, "void (int iChange)")

		.def("getExtraHappiness", &CyPlayer::getExtraHappiness, "int ()")
		.def("changeExtraHappiness", &CyPlayer::changeExtraHappiness, "void (int iChange)")

		.def("getBuildingHappiness", &CyPlayer::getBuildingHappiness, "int ()")
		.def("getWarWearinessPercentAnger", &CyPlayer::getWarWearinessPercentAnger, "int ()")
		.def("getWarWearinessModifier", &CyPlayer::getWarWearinessModifier, "int ()")
		.def("getCoastalTradeRoutes", &CyPlayer::getCoastalTradeRoutes, "int ()")
		.def("changeCoastalTradeRoutes", &CyPlayer::changeCoastalTradeRoutes, "void (int iChange)")
		.def("getTradeRoutes", &CyPlayer::getTradeRoutes, "int ()")

		.def("getRevolutionTimer", &CyPlayer::getRevolutionTimer, "int ()")
		.def("changeRevolutionTimer", &CyPlayer::changeRevolutionTimer, "void (int addTime)")

		.def("isStateReligion", &CyPlayer::isStateReligion, "bool ()")
		.def("getStateReligionHappiness", &CyPlayer::getStateReligionHappiness, "int ()")
		.def("getStateReligionUnitProductionModifier", &CyPlayer::getStateReligionUnitProductionModifier, "int ()")
		.def("changeStateReligionUnitProductionModifier", &CyPlayer::changeStateReligionUnitProductionModifier, "void (int iChange)")
		.def("getStateReligionBuildingProductionModifier", &CyPlayer::getStateReligionBuildingProductionModifier, "int ()")
		.def("changeStateReligionBuildingProductionModifier", &CyPlayer::changeStateReligionBuildingProductionModifier, "void (int iChange)")
		.def("getStateReligionFreeExperience", &CyPlayer::getStateReligionFreeExperience, "int ()")
		.def("getCapitalCity", &CyPlayer::getCapitalCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iID)")
		.def("getCitiesLost", &CyPlayer::getCitiesLost, "int ()")

		.def("getAssets", &CyPlayer::getAssets, "int ()")
		.def("getPower", &CyPlayer::getPower, "int ()")
		.def("getUnitPower", &CyPlayer::getUnitPower, "int ()")
		.def("getPopScore", &CyPlayer::getPopScore, "int ()")
		.def("getLandScore", &CyPlayer::getLandScore, "int ()")

		.def("getSevoWondersScore", &CyPlayer::getSevoWondersScore, "int ()")

		.def("getWondersScore", &CyPlayer::getWondersScore, "int ()")
		.def("getTechScore", &CyPlayer::getTechScore, "int ()")
		.def("isMinorCiv", &CyPlayer::isMinorCiv, "bool ()")

		.def("isAlive", &CyPlayer::isAlive, "bool ()")
		.def("isEverAlive", &CyPlayer::isEverAlive, "bool ()")
		.def("isFoundedFirstCity", &CyPlayer::isFoundedFirstCity, "bool ()")
		.def("isStrike", &CyPlayer::isStrike, "bool ()")

		.def("getID", &CyPlayer::getID, "int ()")
		.def("getHandicapType", &CyPlayer::getHandicapType, "int ()")
		.def("getCivilizationType", &CyPlayer::getCivilizationType, "int ()")
		.def("getLeaderType", &CyPlayer::getLeaderType, "int ()")
		.def("getPersonalityType", &CyPlayer::getPersonalityType, "int ()")
		.def("setPersonalityType", &CyPlayer::setPersonalityType, "void (int /*LeaderHeadTypes*/ eNewValue)")
		.def("getCurrentEra", &CyPlayer::getCurrentEra, "int ()")
		.def("setCurrentEra", &CyPlayer::setCurrentEra, "void (int /*EraTypes*/ iNewValue)")
		.def("getStateReligion", &CyPlayer::getStateReligion, "int ()")
		.def("setLastStateReligion", &CyPlayer::setLastStateReligion, "void (int iReligionID) - Sets the player's state religion to iReligionID")
		.def("getTeam", &CyPlayer::getTeam, "int ()")
		.def("isTurnActive", &CyPlayer::isTurnActive, "bool ()")

		.def("getPlayerColor", &CyPlayer::getPlayerColor, "int (PlayerColorTypes) () - returns the color ID of the player")
		.def("getPlayerTextColorR", &CyPlayer::getPlayerTextColorR, "int ()")
		.def("getPlayerTextColorG", &CyPlayer::getPlayerTextColorG, "int ()")
		.def("getPlayerTextColorB", &CyPlayer::getPlayerTextColorB, "int ()")
		.def("getPlayerTextColorA", &CyPlayer::getPlayerTextColorA, "int ()")

		.def("getYieldRateModifier", &CyPlayer::getYieldRateModifier, "int (YieldTypes eIndex)")
		.def("getCommercePercent", &CyPlayer::getCommercePercent, "int (CommerceTypes eIndex)")
		.def("changeCommercePercent", &CyPlayer::changeCommercePercent, "int (CommerceTypes eIndex, int iChange)")
		.def("getCommerceRate", &CyPlayer::getCommerceRate, "int (CommerceTypes eIndex)")
		.def("getCommerceRateModifier", &CyPlayer::getCommerceRateModifier, "int (CommerceTypes eIndex)")
		.def("getCapitalCommerceRateModifier", &CyPlayer::getCapitalCommerceRateModifier, "int (CommerceTypes eIndex)")
		.def("getSpecialistExtraCommerce", &CyPlayer::getSpecialistExtraCommerce, "int (CommerceTypes eIndex)")
		.def("isCommerceFlexible", &CyPlayer::isCommerceFlexible, "bool (CommerceTypes eIndex)")

		.def("isFeatAccomplished", &CyPlayer::isFeatAccomplished, "bool ()")
		.def("setFeatAccomplished", &CyPlayer::setFeatAccomplished, "void ()")
		.def("isOption", &CyPlayer::isOption, "bool ()")
		.def("setOption", &CyPlayer::setOption, "void ()")
		.def("getVotes", &CyPlayer::getVotes, "int (int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource)")
		.def("isFullMember", &CyPlayer::isFullMember, "bool ()")
		.def("isVotingMember", &CyPlayer::isVotingMember, "bool ()")
		.def("isPlayable", &CyPlayer::isPlayable, "bool ()")
		.def("getBonusExport", &CyPlayer::getBonusExport, "int (CommerceTypes eIndex)")
		.def("getBonusImport", &CyPlayer::getBonusImport, "int (CommerceTypes eIndex)")

		.def("getImprovementCount", &CyPlayer::getImprovementCount, "int (int /*ImprovementTypes*/ iIndex)")

		.def("getExtraBuildingHappiness", &CyPlayer::getExtraBuildingHappiness, "int (int /*BuildingTypes*/ eIndex)")
		.def("getExtraBuildingHealth", &CyPlayer::getExtraBuildingHealth, "int (int /*BuildingTypes*/ eIndex)")

		.def("getUnitCount", &CyPlayer::getUnitCount, "int (int (UnitTypes) eIndex)")
		.def("isUnitMaxedOut", &CyPlayer::isUnitMaxedOut, "bool (int (UnitTypes) eIndex, int iExtra)")
		.def("getUnitMaking", &CyPlayer::getUnitMaking, "int (int (UnitTypes) eIndex)")
		.def("getUnitCountPlusMaking", &CyPlayer::getUnitCountPlusMaking, "int (int (UnitTypes) eIndex)")

		.def("getBuildingCount", &CyPlayer::getBuildingCount, "int (int /*BuildingTypes*/ eIndex)")
		.def("isBuildingMaxedOut", &CyPlayer::isBuildingMaxedOut, "bool (int /*BuildingTypes*/ iIndex, int iExtra)")
		.def("getBuildingCountPlusMaking", &CyPlayer::getBuildingCountPlusMaking, "int (int /*BuildingTypes*/ iIndex)")
		.def("canHurry", &CyPlayer::canHurry, "int (int (HurryTypes) eIndex)")

		.def("getHasReligionCount", &CyPlayer::getHasReligionCount)
		.def("getHasCorporationCount", &CyPlayer::getHasCorporationCount)

		.def("isSpecialistValid", &CyPlayer::isSpecialistValid, "bool (int /*SpecialistTypes*/ iIndex)")
		.def("isResearchingTech", &CyPlayer::isResearchingTech, "bool (int /*TechTypes*/ iIndex)")
		.def("getCivics", &CyPlayer::getCivics, "int /*CivicTypes*/ (int /*CivicOptionTypes*/ iIndex)")
		.def("getSingleCivicUpkeep", &CyPlayer::getSingleCivicUpkeep, "int (int /*CivicTypes*/ eCivic, bool bIgnoreAnarchy)")
		.def("getCivicUpkeep", &CyPlayer::getCivicUpkeep, "int (bool bIgnoreAnarchy)")
		.def("setCivics", &CyPlayer::setCivics, "void (int iCivicOptionType, int iCivicType) - Used to forcibly set civics with no anarchy")

		.def("getCombatExperience", &CyPlayer::getCombatExperience, "int () - Combat experience used to produce Warlords")
		.def("changeCombatExperience", &CyPlayer::changeCombatExperience, "void (int) - Combat experience used to produce Warlords")
		.def("setCombatExperience", &CyPlayer::setCombatExperience, "void (int) - Combat experience used to produce Warlords")

		.def("getQueuePosition", &CyPlayer::getQueuePosition, "int")
		.def("clearResearchQueue", &CyPlayer::clearResearchQueue, "void ()")
		.def("pushResearch", &CyPlayer::pushResearch, "void (int /*TechTypes*/ iIndex, bool bClear)")
		.def("getLengthResearchQueue", &CyPlayer::getLengthResearchQueue, "int ()")

		.def("addCityName", &CyPlayer::addCityName, "void (std::wstring szName)")
		.def("getNumCityNames", &CyPlayer::getNumCityNames, "int ()")
		.def("getCityName", &CyPlayer::getCityName, "std::wstring (int iIndex)")

		.def("cities", &CyPlayer::cities)
		.def("firstCity", &CyPlayer::firstCity, "tuple(CyCity, int iterOut) (bool bReverse) - gets the first city")
		.def("nextCity", &CyPlayer::nextCity, "tuple(CyCity, int iterOut) (int iterIn, bool bReverse) - gets the next city")
		.def("getNumCities", &CyPlayer::getNumCities, "int ()")
		.def("getCity", &CyPlayer::getCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iID)")

		.def("units", &CyPlayer::units)
		.def("firstUnit", &CyPlayer::firstUnit, "tuple(CyUnit, int iterOut) (bool bReverse) - gets the first unit")
		.def("nextUnit", &CyPlayer::nextUnit, "tuple(CyUnit, int iterOut) (int iterIn, bool bReverse) - gets the next unit")
		.def("getNumUnits", &CyPlayer::getNumUnits, "int ()")
		.def("getUnit", &CyPlayer::getUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iID)")

		.def("groups", &CyPlayer::groups)
		.def("getNumSelectionGroups", &CyPlayer::getNumSelectionGroups, "int ()")
		.def("getSelectionGroup", &CyPlayer::getSelectionGroup, python::return_value_policy<python::manage_new_object>(), "CvSelectionGroup* (int iID)")

		.def("trigger", &CyPlayer::trigger, "void (/*EventTriggerTypes*/int eEventTrigger)")
		.def("getEventOccured", &CyPlayer::getEventOccured, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int /*EventTypes*/ eEvent)")
		.def("resetEventOccured", &CyPlayer::resetEventOccured, "void (int /*EventTypes*/ eEvent)")
		.def("getEventTriggered", &CyPlayer::getEventTriggered, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int iID)")
		.def("initTriggeredData", &CyPlayer::initTriggeredData, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, ReligionTypes eReligion, CorporationTypes eCorporation, int iUnitId, BuildingTypes eBuilding)")
		.def("getEventTriggerWeight", &CyPlayer::getEventTriggerWeight, "int getEventTriggerWeight(int eEventTrigger)")
	;
}
